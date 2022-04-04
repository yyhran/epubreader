#include "edocument.h"

namespace EPUB {

Document::Document(const QString& fileName, QObject* parent)
    : QTextDocument(parent)
    , _fileName(fileName)
    , _baseRefDir("")
    , _openedFile("")
    , _runOnRam(false)
    , _opened(false)
{
    if(not this->_runOnRam)
    {
        QFileInfo fileInfo(fileName);
        QString name = fileInfo.baseName();
        this->_bookPath = QCoreApplication::applicationDirPath() + "/books/" + name + "/";
        QDir dir(this->_bookPath);
        if(not dir.exists()) dir.mkpath(this->_bookPath);
    }
    if(this->open())
    {
        if("" != this->_coverImage)
        {
            this->setHtml(this->_coverImage);
        }
        else this->setFile("titlepage.xhtml");
    }
    
}

Document::~Document()
{ 
    if(not this->_runOnRam)
    {
        Q_ASSERT("" != this->_bookPath);
        QDir dir(this->_bookPath);
        if(dir.exists())
        {
            dir.removeRecursively();
        }
    }
}

auto Document::open() -> bool
{
    UnZipStream* unZip = new UnZipStream(this->_fileName);

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
                this->_textData.insert(name, it.value());
            }
            else
            {
                QFileInfo fileInfo(name);
                if(QLatin1String("image/jpeg") == mimeType)
                {
                    if(name.contains("/")) this->_imgPath = this->_bookPath + name.left(name.lastIndexOf("/")) + "/";
                    if(this->_runOnRam)
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
                    this->_metaData.insert(name, data);
                }
                if(not this->_runOnRam) this->saveFile(name, data);
            }
        }
    }
    delete unZip;
    unZip = nullptr;

    for(auto it = this->_textData.begin(); it != this->_textData.end(); ++it)
    {
        this->changePath(it.key(), it.value());
    }

    for(auto it = this->_metaData.begin(); it != this->_metaData.end(); ++it)
    {
        if(METAINFOCONTAINERFILE == it.key())
        {
            if(it.value().size() < 4) return false;
            if(this->metaReader(it.value()))
            {
                this->_opened = true;
                return true;
            }
        }
    }

    return false;
}

auto Document::setFile(const QString& file) -> void
{
    this->_openedFile = file;
    if(this->_runOnRam)
    {
        for(auto it = this->_textData.begin(); it != this->_textData.end(); ++it)
        {
            if(it.key().contains(file))
            {
                this->setHtml(it.value());
            }
        }
    }
    else
    {
        QFile f(this->_bookPath + file);
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
    QDomElement root = this->getDomElementFromXml(this->_metaData[containerFile], true);
    QDomNodeList der = root.elementsByTagName("rootfile");
    if(0 == der.count())
    {
        EPUBWARNING() << "unable to get place from file content.opf, inside META-INF/container.xml" << __LINE__;
        return ret;
    }
    this->_metaData.remove(containerFile);
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
        this->_baseRefDir = contentFile.left(contentFile.lastIndexOf("/")) + "/";
    }

    QDomNodeList metaList = this->getPageName(contentFile, QString("metadata"));
    QDomNodeList list = metaList.at(0).toElement().childNodes();
    for(int i{0}; i < list.size(); ++i)
    {
        QDomNode&& tmp = list.at(i);
        if("dc" == tmp.prefix())
        {
            this->_metaInfo[tmp.localName()] << tmp.firstChild().toText().data();
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
            if(not this->_runOnRam)
            {
                if("image/jpeg" == nodepager.attribute("media-type"))
                {
                    this->_coverImage = "<img src=\"" + this->_bookPath + this->_baseRefDir + nodepager.attribute("href") +"\"/>";
                }
                else if("application/xhtml+xml" == nodepager.attribute("media-type"))
                {
                    QFile f(this->_bookPath + this->_baseRefDir + nodepager.attribute("href"));
                    if(f.exists())
                    {
                        if(f.open(QIODevice::ReadOnly))
                        {
                            this->_coverImage = f.readAll().toStdString().c_str();
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
        tocFile = this->_baseRefDir + tocFile;
    }

    EPUBDEBUG() << "get toc from  " << tocFile << __LINE__;

    QDomNodeList navitom = this->getPageName(tocFile, "navMap");

    if (1 == navitom.count())
    {
        QDomElement elem = navitom.at(0).toElement();
        ret = this->readMenu(elem);
    }
    this->_metaData.remove(tocFile);

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
            toc.src = this->_baseRefDir + tmp.nextSiblingElement().attribute("src");
            this->_toc.append(toc);
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
            // QString path = (this->_runOnRam ? "myData:/" : this->_cssPath) + fileInfo.fileName();
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
        QString path = (this->_runOnRam ? "myData:/" : this->_imgPath) + fileInfo.fileName();
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
        QString path = (this->_runOnRam ? "myData:/" : this->_bookPath) + fileInfo.fileName();
        QDomElement imagElement = document.createElement("img");
        imagElement.setAttribute("src", path);
        QDomNode parent = svgList.at(i).parentNode();
        parent.replaceChild(imagElement, svgNode);
    }

    if(this->_runOnRam)
    {
        xml.clear();
        QTextStream out(xml);
        document.save(out, 2);
    }
    else
    {
        QString savePath = this->_bookPath + name;
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

    QByteArray chunx = this->_metaData[fileName]; 
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
    QString savePath = this->_bookPath + name;
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