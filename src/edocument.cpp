#include "edocument.h"

namespace EPUB {

Document::Document(const QString& fileName, QObject* parent)
    : QTextDocument(parent)
    , fileName_(fileName)
    , baseRefDir_("")
    , openedFile_("")
    , runOnRam_(false)
    , opened_(false)
{
    if(not this->runOnRam_)
    {
        QFileInfo fileInfo(fileName);
        QString name = fileInfo.baseName();
        this->bookPath_ = QCoreApplication::applicationDirPath() + "/books/" + name + "/";
        QDir dir(this->bookPath_);
        if(not dir.exists()) dir.mkpath(this->bookPath_);
    }
    if(this->open())
    {
        if("" != this->coverImage_)
        {
            this->setHtml(this->coverImage_);
        }
        else this->setFile("titlepage.xhtml");
    }
    
}

Document::~Document()
{ 
    if(not this->runOnRam_)
    {
        Q_ASSERT("" != this->bookPath_);
        QDir dir(this->bookPath_);
        if(dir.exists())
        {
            dir.removeRecursively();
        }
    }
}

auto Document::open() -> bool
{
    UnZipStream* unZip = new UnZipStream(this->fileName_);

    if(unZip->canRead())
    {
        // save files to local, if don't on ram
        DataMap allFiles = unZip->listData();
        for(auto it = allFiles.begin(); it != allFiles.end(); ++it)
        {
            QByteArray chunk = it.value();

            QString name(it.key());
            EPUBDEBUG() << "file zip in: " << name;

            QString mimeType = MimeinFile(name);
            QByteArray& data = it.value();

            if(QLatin1String("text/html") == mimeType or QLatin1String("application/xhtml+xml") == mimeType)
            {
                this->textData_.insert(name, it.value());
            }
            else
            {
                QFileInfo fileInfo(name);
                if(QLatin1String("image/jpeg") == mimeType)
                {
                    if(name.contains("/")) this->imgPath_ = this->bookPath_ + name.left(name.lastIndexOf("/")) + "/";
                    if(this->runOnRam_)
                    {
                        this->addResource(QTextDocument::ImageResource, QUrl("myData:/" + fileInfo.fileName()), data);
                    }
                }
                else if(QLatin1String("text/css") == mimeType)
                {                                    
                    this->addResource(QTextDocument::StyleSheetResource, QUrl("myData:/" + fileInfo.fileName()), data);
                }
                else 
                {
                    this->metaData_.insert(name, data);
                }
                if(not this->runOnRam_) this->saveFile(name, data);
            }
        }
    }
    delete unZip;
    unZip = nullptr;

    for(auto it = this->textData_.begin(); it != this->textData_.end(); ++it)
    {
        this->changePath(it.key(), it.value());
    }

    for(auto it = this->metaData_.begin(); it != this->metaData_.end(); ++it)
    {
        if("META-INF/container.xml" == it.key())
        {
            if(it.value().size() < 4) return false;
            if(this->metaReader(it.value()))
            {
                this->opened_ = true;
                return true;
            }
        }
    }

    return false;
}

auto Document::setFile(const QString& file) -> void
{
    this->openedFile_ = file;
    if(this->runOnRam_)
    {
        for(auto it = this->textData_.begin(); it != this->textData_.end(); ++it)
        {
            if(it.key().contains(file))
            {
                this->setHtml(it.value());
            }
        }
    }
    else
    {
        QFile f(this->bookPath_ + file);
        if(f.exists())
        {
            if(f.open(QIODevice::ReadWrite))
            {
                this->setHtml(f.readAll());
            }
        }
    }
}

auto Document::getDomElementFromXml(const QByteArray& xml, bool usenamespace) -> QDomElement
{
    QDomDocument root;
    if(xml.size() < 5) return root.createElement("root");

    QDomDocument document;
    if(document.setContent(xml, usenamespace))
    {
        QDomElement node = document.documentElement();
        EPUBDEBUG() << "sucess getDomElementFromXml first tagName: " << node.tagName() << __LINE__;
        return node;
    }

    return root.createElement("root");
}

auto Document::metaReader(QByteArray& xml) -> bool
{
    QString containerFile{"META-INF/container.xml"};
    bool ret{false};
    QDomElement root = this->getDomElementFromXml(this->metaData_[containerFile], true);
    QDomNodeList der = root.elementsByTagName("rootfile");
    if(0 == der.count())
    {
        EPUBWARNING() << "unable to get place from file content.opf, inside META-INF/container.xml" << __LINE__;
        return ret;
    }
    this->metaData_.remove(containerFile);
    QString contentFile{""};

    for(int i{0}; i < der.count(); ++i) 
    {
        QDomElement node = der.at(i).toElement();
        contentFile = node.attribute("full-path"); // content.opf
        if(0 == contentFile.size())
        {
            EPUBWARNING() << "unable to get place from file content.opf, inside META-INF/container.xml" << __LINE__;
            return false;
        }
        EPUBDEBUG() << "Start on file:" << contentFile << __LINE__;
    }

    if(contentFile.contains("/"))
    {
        this->baseRefDir_ = contentFile.left(contentFile.lastIndexOf("/")) + "/";
    }

    QDomNodeList metaList = this->getPageName(contentFile, QString("metadata"));
    QDomNodeList list = metaList.at(0).toElement().childNodes();
    for(int i{0}; i < list.size(); ++i)
    {
        QDomNode&& tmp = list.at(i);
        if("dc" == tmp.prefix())
        {
            this->metaInfo_[tmp.localName()] << tmp.firstChild().toText().data();
        }
    }
    // QDomNodeList guideList = this->getPageName(contentFile, QString("guide"));
    // this->_coverPager = guideList.at(0).toElement().firstChildElement().attribute("htef");

    QString tocFile{""};
    QDomNodeList itemlist = this->getPageName(contentFile, QString("item"));
    for(int i{0}; i < itemlist.count(); ++i) 
    {
        QDomElement nodepager = itemlist.at(i).toElement();
        QString attrId = nodepager.attribute("id");
        if("ncx" == attrId)
        {
            tocFile = nodepager.attribute("href");
        }
        if(attrId.contains("cover"))
        {
            if(not this->runOnRam_)
            {
                if("image/jpeg" == nodepager.attribute("media-type"))
                {
                    this->coverImage_ = "<img src=\"" + this->bookPath_ + this->baseRefDir_ + nodepager.attribute("href") +"\"/>";
                }
                else if("application/xhtml+xml" == nodepager.attribute("media-type"))
                {
                    QFile f(this->bookPath_ + this->baseRefDir_ + nodepager.attribute("href"));
                    if(f.exists())
                    {
                        if(f.open(QIODevice::ReadOnly))
                        {
                            this->coverImage_ = f.readAll().toStdString().c_str();
                        }
                    }
                }
            }
        }
    }
    if(0 == tocFile.size())
    {
        EPUBDEBUG() << "unable to get place from file toc.ncx, inside content.opf" << __LINE__;
        return false;
    }
    else
    {
        tocFile = this->baseRefDir_ + tocFile;
    }

    EPUBDEBUG() << "get toc from  " << tocFile << __LINE__;

    QDomNodeList navitom = this->getPageName(tocFile, "navMap");

    if (1 == navitom.count())
    {
        QDomElement elem = navitom.at(0).toElement();
        ret = this->readMenu(elem);
    }
    this->metaData_.remove(tocFile);

    return ret;
}

auto Document::readMenu(const QDomElement& element, const QString& text) -> bool
{
    QDomElement child = element.firstChildElement();
    while(not child.isNull())
    {
        if("navPoint" == child.tagName())
        {
            EpubToc toc;
            toc.upper = text;
            toc.order = child.attribute("playOrder").toInt();
            QDomElement tmp = child.firstChildElement();
            toc.text = tmp.firstChildElement().firstChild().toText().data();
            toc.src = this->baseRefDir_ + tmp.nextSiblingElement().attribute("src");
            this->toc_.append(toc);
            this->readMenu(child, toc.text); // read submenu
        }
        child = child.nextSiblingElement();
    }
    return true;
}

auto Document::changePath(const QString& name, QByteArray& xml) -> void
{
    QDomDocument document;
    document.setContent(xml);
    QDomElement root = document.documentElement();
    // change css path
    QDomNodeList cssList = root.elementsByTagName("link");
    for(int i{0}; i < cssList.size(); ++i)
    {
        QDomElement oldNode = cssList.at(i).toElement();
        if("text/css" == oldNode.attribute("type"))
        {
            QFileInfo fileInfo(oldNode.attribute("href"));
            // QString path = (this->runOnRam_ ? "myData:/" : this->_cssPath) + fileInfo.fileName();
            QString path = "myData:/" + fileInfo.fileName();
            oldNode.setAttribute("href", path); 
            QDomElement newNode = cssList.at(i).toElement();
            cssList.at(i).replaceChild(newNode, oldNode);
        }
    }
    // change image path
    QDomNodeList imgList = root.elementsByTagName("img");
    for(int i{0}; i < imgList.size(); ++i)
    {
        QDomElement oldNode = imgList.at(i).toElement();
        QFileInfo fileInfo(oldNode.attribute("src"));
        QString path = (this->runOnRam_ ? "myData:/" : this->imgPath_) + fileInfo.fileName();
        oldNode.setAttribute("src", path);
        QDomElement newNode = imgList.at(i).toElement();
        imgList.at(i).replaceChild(newNode, oldNode); 
    }
    // change <svg> to <img>
    QDomNodeList svgList = root.elementsByTagName("svg");
    for(int i{0}; i < svgList.size(); ++i)
    {
        QDomElement svgNode = svgList.at(i).toElement();
        QDomDocument tmpDom;
        tmpDom.appendChild(tmpDom.importNode(svgNode, true));

        QFileInfo fileInfo(svgNode.elementsByTagName("image").at(0).toElement().attribute("xlink:href"));
        QString path = (this->runOnRam_ ? "myData:/" : this->bookPath_) + fileInfo.fileName();
        QDomElement imagElement = document.createElement("img");
        imagElement.setAttribute("src", path);
        QDomNode parent = svgList.at(i).parentNode();
        parent.replaceChild(imagElement, svgNode);
    }

    if(this->runOnRam_)
    {
        xml.clear();
        QTextStream out(xml);
        document.save(out, 2);
    }
    else
    {
        QString savePath = this->bookPath_ + name;
        QString dirPath = savePath.left(savePath.lastIndexOf("/")) + "/";
        QDir dir(dirPath);
        if(not dir.exists()) dir.mkpath(dirPath);

        QFile file(savePath);
        if(file.open(QIODevice::WriteOnly))
        {
            QTextStream out(&file);
            document.save(out, 2);
        }
    }
}

auto Document::getPageName(const QString fileName, const QString tag) -> QDomNodeList
{
    EPUBDEBUG() << "Request GetPageName: file_name/tag" << fileName
                << " : " << tag
                << " current actioncycle." << __LINE__;

    QDomNodeList defineterror;

    QByteArray chunx = this->metaData_[fileName]; 
    if(0 == chunx.size()) return defineterror;

    QDomElement e = this->getDomElementFromXml(chunx, true);
    QDomNodeList der = e.elementsByTagName(tag);
    QDomNodeList dera = e.elementsByTagName(e.tagName());

    if(der.count() > 0) return der;
    else if(dera.count() > 0) return dera;

    EPUBDEBUG() << "Request Error: " << fileName
                << ":" << tag
                << " export FAIL!...." << __LINE__;
    return defineterror;
}

auto Document::saveFile(const QString& name, QByteArray& data) -> void
{
    QString savePath = this->bookPath_ + name;
    QString dirPath = savePath.left(savePath.lastIndexOf("/")) + "/";
    QDir dir(dirPath);
    if(not dir.exists()) dir.mkpath(dirPath);

    QFile file(savePath);
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(data);
    }
}

} // namespace EPUB