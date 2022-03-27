#include "edocument.h"

namespace EPUB {

Document::Document(const QString& fileName, QObject* parent)
    : QTextDocument(parent)
    , _fileName(fileName)
    , _coverPager("")
    , _startIndexPager("")
    , _useBaseRef(false)
    , _summerror(0)
    , _minNrorder(10000)
    , _maxNrorder(0)
    , _compressOnRam(false)
{
    QFileInfo fileInfo(fileName);
    QString name = fileInfo.baseName();
    this->_bookPath = QCoreApplication::applicationDirPath() + "/books/" + name + "/";
    EPUBDEBUG() << "fileName: " << name;
    EPUBDEBUG() << "bookPath: " << this->_bookPath;
}

Document::~Document() { }

auto Document::open() -> bool
{
    UnZipStream* unZip = new UnZipStream(this->_fileName);
    QByteArray tableOfContent;
    QByteArray fileListener;
    QByteArray metaIndoFile;

    if(unZip->canRead())
    {
        const QStringList entries = unZip->fileList();
        DataMap allFiles = unZip->listData();
        for(auto it = allFiles.begin(); it != allFiles.end(); ++it)
        {
            QByteArray chunk = it.value();

            QString name(it.key());
            EPUBDEBUG() << "file zip in: " << name;
            QString mimeType = MimeinFile(name);
            if(QLatin1String("text/html") == mimeType or QLatin1String("application/xhtml+xml") == mimeType)
            {
                this->_uniqueuris.insert(name);
                this->_textData.insert(name, it.value());
            }
            else if(QLatin1String("image/jpeg") == mimeType)
            {
               this->_imageData.insert(name, it.value()); 
            }
            else if(QLatin1String("text/css") == mimeType)
            {
                this->_styleData.insert(name, it.value());
            }
            else if(METAINFOCONTAINERFILE == name)
            {
                this->_nextFileToReadXmlChunk = chunk;
            }
            this->_cache.insert(name, chunk);
            this->saveFile(name, it);
        }
    }

    delete unZip;
    unZip = nullptr;

    if(this->_nextFileToReadXmlChunk.size() < 4) return false;
    else
    {
        bool ojbk = this->metaReader();
        if(not this->_baseRefDir.isEmpty())
        {
            EPUBDEBUG() << "BASEREFDIR: " << this->_baseRefDir;
        }
        {
            EPUBDEBUG() << "rspine: " << this->_rspine << '\n'
                        << "coverPager: " << this->_coverPager << '\n'
                        << "fileTiele: " << this->_fileTitle << '\n'
                        << "minNrorder: " << this->_minNrorder << '\n'
                        << "maxNrorder: " << this->_maxNrorder << '\n'
                        << "pageItem: " << this->_pageItem.size() << '\n'
                        << "menuItem: " << this->_menuItem.size() << '\n'
                        << "rspine size: " << this->_rspine.size() << '\n';
        }
        EPUBDEBUG() << "total error: " << this->_summerror;
        if(this->_summerror > 0)
        {
            EPUBDEBUG() << "code error: " << this->_recError;
        }

        if(ojbk && this->_summerror < 3) this->pageBuilder();
        if(this->_revisionPageItem.size() > 1) return true;
    }
    return false;
}

auto Document::setEpubError(const QString& msg) -> void
{
    EPUBDEBUG() << "new error: " << msg;
    ++this->_summerror;
    this->_recError << msg;
}

auto Document::make2DomElementXmlFile(const QByteArray xml) -> QDomElement
{
    EPUBDEBUG() << "make2DomElementXmlFile fiel: " << this->_nextFileToReadCrono;

    QDomDocument root;
    if(xml.size() < 5) return root.createElement("root");

    QDomElement node;
    QXmlSimpleReader reader;
    QXmlInputSource source;
    if(this->_compressOnRam) source.setData(qUncompress(xml));
    else source.setData(xml);

    QString eErrorMsg;
    QDomDocument document;
    if(document.setContent(&source, &reader, &eErrorMsg))
    {
        node = document.documentElement();
        EPUBDEBUG() << "sucess make2DomElementXmlFile first tagName: " << node.tagName();
        return node;
    }
    else
    {
        this->setEpubError(QString("make2DomElementXmlFile unable to read xml file ... %1").arg(eErrorMsg));
    }
    return root.createElement("root");
}

auto Document::images() const -> DataMap { return this->_imageData; }

auto Document::structure() const -> DataMap { return this->_cache; }

auto Document::removeFromRam(const QString fileName) -> bool
{
    EPUBDEBUG() << "remove file: " << fileName << " .";
    for(auto&& it = this->_cache.begin(); it != this->_cache.end(); ++it)
    {
        if(fileName == it.key())
        {
            int ret = this->_cache.remove(fileName); 
            if(1 == ret)
            {
                EPUBDEBUG() << "sucess remove from ram file: " << fileName << " .";
                return true;
            }
            else
            {
                this->setEpubError(QString("unable to remove: %2 from ram cache.").arg(fileName));
                return false;
            }
        }
    }
    return false;
}

auto Document::lostFoundSyncro() -> void
{
    for(auto&& fox : this->_pageItem)
    {
        const QString urlZip = fox.qurl();
        if(this->_uniqueuris.contains(urlZip))
        {
            this->_uniqueuris.remove(urlZip);
        }

        EPUBDEBUG() << "page item set: " << fox.debug();
    }

    for(auto&& fox : this->_menuItem)
    {
        const QString urlZip = fox.qurl();
        if(this->_uniqueuris.contains(urlZip))
        {
            this->_uniqueuris.remove(urlZip);
        }

        EPUBDEBUG() << "menu item set: " << fox.debug();
    }

    this->_imageData.clear();
    this->_cache.clear();
}

auto Document::pageBuilder() -> void
{
    this->lostFoundSyncro();
    QSet<QString> uniqueurlSet;
    QString zUrl;
    QString tmp1, tmp2;
    QString idRef;
    const int pageNum{ (this->_maxNrorder - this->_minNrorder) + 2 };
    const int epubSize{ this->_menuItem.size() };
    const int qustSize{ this->_uniqueuris.size() };
    const int pagesSize{ this->_pageItem.size() };
    EPUBDEBUG() << "pageBuilder tot: " << pagesSize
                << ", diff: " << pageNum
                << ": " << epubSize
                << " - remainfile" << qustSize;
    EpubToc coverfox;
    for(int i{0}; i < this->_rspine.count(); ++i)
    {
        idRef = this->_rspine.at(i);
        EpubToc fox;
        EpubToc foxMenu;
        this->getPageOrderID(idRef, fox);
        fox.orderid = i;
        this->getPageKeyMd843(fox.md843, foxMenu);
        tmp1 = this->_bookPath + fox.qurl();
        QFileInfo fi(tmp1);
        const QString boxUrl = fi.absoluteFilePath();
        if(fi.exists() and not uniqueurlSet.contains(boxUrl))
        {
            fox.jumpUrl = boxUrl;
            uniqueurlSet.insert(boxUrl);
            fox.title = foxMenu.title;
            if(fox.title.isEmpty())
            {
                fox.title = QString("Page - Unkonw Title / or not set");
            }
            this->_revisionPageItem.append(fox);
            EPUBDEBUG() << this->_revisionPageItem.size()
                        << "/" << idRef
                        << ") GoTo refister: " << fox.debug()
                        << "_" << fi.absoluteFilePath();
        }
        int listers{-1};
        for(auto&& fox :  this->_pageItem)
        {
            ++listers;
            tmp1 = this->_bookPath + fox.qurl();
            QFileInfo fi(tmp1);
            const QString boxUrl = fi.absoluteFilePath();
            if(fi.exists() and not uniqueurlSet.contains(boxUrl))
            {
                fox.orderid = this->_revisionPageItem.count();
                fox.jumpUrl = boxUrl;
                uniqueurlSet.insert(boxUrl);
                fox.title = QString("Hidden/Listed page: %1").arg(fox.title);
                if(fox.title.isEmpty())
                {
                    fox.title = QString("Page - Unkonw Title / or not set");
                }
                this->_revisionPageItem.append(fox);
                EPUBDEBUG() << this->_revisionPageItem.size()
                            << "/" << idRef
                            << ") GoTo refister: " << fox.debug()
                            << "_" << fi.absoluteFilePath();                
            }
        }
    }
    QStringList lostList = uniqueurlSet.toList();
    EPUBDEBUG() << "!!! lost & found size: " << lostList.size();
    for(auto&& lost: lostList)
    {
        EPUBDEBUG() << "!!! lost & found page: " << lost;
    }
}

auto Document::getPageKeyMd843(const int idmd, EpubToc& item) -> void
{
    for(auto&& fox : this->_menuItem)
    {
        if(idmd == fox.md843)
        {
            if(this->_uniqueuris.contains(fox.qurl()))
            {
                this->_uniqueuris.remove(fox.qurl());
            }
            item = fox;
        }
    }
}

auto Document::getMenuOrderID(const int x, EpubToc& item) -> void
{
    for(auto&& fox : this->_menuItem)
    {
        if(x == fox.orderid)
        {
            if(this->_uniqueuris.contains(fox.qurl()))
            {
                this->_uniqueuris.remove(fox.qurl());
            }
            item = fox;
        }
    }
}

auto Document::getPageOrderID(const QString ref, EpubToc& item) -> void
{
    bool found{false};
    for(auto&& fox : this->_pageItem)
    {
        if(ref == fox.idref)
        {
            if(this->_uniqueuris.contains(fox.qurl()))
            {
                this->_uniqueuris.remove(fox.qurl());
            }
            found = true;
            item = fox;
        }
    }
    if(not found)
    {
        for(auto&& fox : this->_menuItem)
        {
            if(ref == fox.idref)
            {
                if(this->_uniqueuris.contains(fox.qurl()))
                {
                    this->_uniqueuris.remove(fox.qurl());
                }
                found = true;
                item = fox;
            }
        }
    }
    if(not found)
    {
        this->setEpubError(QString("unable to get id from page %1").arg(ref));
    }
}

auto Document::metaReader() -> bool
{
    EPUBDEBUG() << "meta reader only read & follow:" << METAINFOCONTAINERFILE
                << "function: " << __FUNCTION__
                << " line:" << __LINE__;
    const QByteArray xml = this->_nextFileToReadXmlChunk;
    this->_nextFileToReadCrono = METAINFOCONTAINERFILE;
    bool xvalid{false};
    QString tmp0, tmp1, tmp2;
    QString uricontents;
    QDomElement root = this->make2DomElementXmlFile(this->_nextFileToReadXmlChunk);
    QDomNodeList der = root.elementsByTagName("rootfile");
    if(der.count() < 1)
    {
        EPUBDEBUG() << __FUNCTION__ << " ERROR: der.count():" << der.count() << " line:" << __LINE__;
        this->setEpubError(QString("unable to get place from file content.opf, inside META-INF/container.xml"));
        return xvalid;
    }

    this->removeFromRam(this->_nextFileToReadCrono);
    for(int i{0}; i < der.count(); ++i) 
    {
        QDomElement node = der.at(i).toElement();
        this->_nextFileToReadCrono = node.attribute("full-path");
        EPUBDEBUG() << "Start on file:" << this->_nextFileToReadCrono << __LINE__;
    }

    if(this->_nextFileToReadCrono.size() < 1)
    {
        this->setEpubError(QString("unable to get place from file content.opf, inside META-INF/container.xml"));
        return false;
    }

    if(this->_nextFileToReadCrono.contains("/"))
    {
        this->_baseRefDir = this->_nextFileToReadCrono.left(this->_nextFileToReadCrono.lastIndexOf("/")) + "/";
    }

    QDomNodeList itemlist = this->getPageName(this->_nextFileToReadCrono, QString("item"));
    for(int i{0}; i < itemlist.count(); ++i) 
    {
        QDomElement nodepager = itemlist.at(i).toElement();
        if (this->fileListRecord(nodepager)) xvalid = true;
        else return false;
    }

    if(this->_useBaseRef)
    {
        this->_nextFileToReadCrono = this->_useBaseRef + QString("toc.ncx");
    }
    else
    {
        this->_nextFileToReadCrono = QString("toc.ncx");
    }

    EPUBDEBUG() << "get toc from  " << this->_nextFileToReadCrono << __LINE__;
    QDomNodeList reffitems = this->getPageName(this->_nextFileToReadCrono, "docTitle");
    QDomNodeList navitom = this->getPageName(this->_nextFileToReadCrono, "navMap");
    if(1 == reffitems.count())
    {
        QDomElement er = reffitems.at(0).toElement();
        QDomElement ed = er.firstChildElement();
        this->_fileTitle = ed.firstChild().toText().data();
    }
    if (1 == navitom.count())
    {
        QDomElement erlem = navitom.at(0).toElement();
        xvalid = this->readMenu(erlem);
    }
    EPUBDEBUG() << "get toc.ncx  end delete navMap " << this->_nextFileToReadCrono << __LINE__;
    this->removeFromRam(this->_nextFileToReadCrono);
    this->_nextFileToReadCrono = this->_baseRefDir + CONTENENTOPFFILE;
    EPUBDEBUG() << "search  " << CONTENENTOPFFILE << __LINE__;
    if(this->cacheFinder(CONTENENTOPFFILE))
    {
        this->_nextFileToReadCrono = CONTENENTOPFFILE;
    }
    else if (this->cacheFinder(this->_nextFileToReadCrono))
    {
        xvalid = true;
    }
    else
    {
        this->setEpubError(QString("Unable to find file : %1").arg(CONTENENTOPFFILE));
        return false;
    }

    QDomNodeList dcover = this->getPageName(this->_nextFileToReadCrono, "reference");
    if(dcover.count() > 0)
    {
        for(int i{0}; i < dcover.count(); ++i)
        {
            QDomElement node = dcover.at(i).toElement();
            if(QLatin1String("reference") == node.tagName())
            {
                EpubToc dax;
                QString type = node.attribute("type");
                QString hrefi = node.attribute("href");
                dax.idref = node.attribute("id");
                if(this->_uniqueuris.contains(hrefi))
                {
                    this->_uniqueuris.remove(hrefi);
                }
                dax.reserve = hrefi;
                if (this->_useBaseRef)
                {
                    hrefi = this->_baseRefDir + node.attribute("href");
                }
                if(this->_uniqueuris.contains(hrefi))
                {
                    this->_uniqueuris.remove(hrefi);
                }

                dax.title = node.attribute("title");
                dax.jumpUrl = QLatin1String("top");
                dax.zipUrl = hrefi;
                dax.md843 = genKeyUrl(hrefi);
                if(QLatin1String("toc") == type)
                {
                    dax.type = QString("toc");
                    dax.orderid = 2;
                    this->_startIndexPager = dax.title + QString("|") + dax.zipUrl;
                }
                else if(QLatin1String("cover") == type)
                {
                    dax.type = QString("cover");
                    dax.orderid = 1;
                    this->_coverPager = dax.title + QString("|") + dax.zipUrl;
                }
                this->_menuItem.append(dax);
            }
        }
    }

    QDomNodeList dspine = this->getPageName(this->_nextFileToReadCrono, "itemref");
    if(dspine.count() > 0)
    {
        for(int i{0}; i < dspine.count(); ++i)
        {
            QDomElement node = dspine.at(i).toElement();
            if (node.tagName() == QLatin1String("itemref"))
            {
                QString orderidx = node.attribute("idref");
                if(orderidx.isEmpty())
                {
                    this->setEpubError(QString("Not found  idref to over or init page!"));
                    return false;
                }
                if(this->_rspine.size() < this->_pageItem.size())
                {
                    this->_rspine << orderidx;
                }
            }
        }
    }

    this->removeFromRam(this->_nextFileToReadCrono);
    if(this->_coverPager.isEmpty())
    {
        this->setEpubError(QString("Coverpage or StartPage variable is not full! from file %1").arg(this->_nextFileToReadCrono));
    }
    else
    {
        xvalid = true;
    }
    if(this->_startIndexPager.isEmpty())
    {
        this->_startIndexPager = QString("takefirstpage_cmd");
    }
    if (this->_coverPager.isEmpty())
    {
        this->_coverPager = QString("makecoer_cmd");
    }
    return xvalid;
}

auto Document::cacheFinder(const QString findFile) -> bool
{
    EPUBDEBUG() << "CacheFinder:" << findFile << " .";

    bool havigness = false;
    for(auto&& it = this->_cache.begin(); it != this->_cache.end(); ++it)
    {
        if(it.key() == findFile)
        {
            havigness = true;
            break;
        }
    }
    return havigness;
}

auto Document::readMenu(const QDomElement& element) -> bool
{
    QDomElement child = element.firstChildElement();

    while(not child.isNull())
    {
        if(child.tagName() == QLatin1String("navPoint"))
        {
            this->_flyID = child.attribute("id");
            this->_flyOrder = child.attribute("playOrder");
            this->readMenu(child);
        }
        else if(child.tagName() == QLatin1String("content"))
        {
            if(this->_useBaseRef)
            {
                this->_flyUrl = this->_baseRefDir + child.attribute("src");
            }
            else
            {
                this->_flyUrl = child.attribute("src");
                if (this->_uniqueuris.contains(this->_flyUrl))
                {
                    this->_uniqueuris.remove(this->_flyUrl);
                }
            }
            bool ok;
            const int idx = this->_flyOrder.toInt(&ok);
            QString keyuu, urti, tmpurl;
            if(ok)
            {
                if(this->_flyUrl.contains("#"))
                {
                    keyuu = this->_flyUrl.left(this->_flyUrl.lastIndexOf("#"));
                    QStringList io = this->_flyUrl.split(QString("#"));
                    urti = io.at(1);
                    if (this->_uniqueuris.contains(urti))
                    {
                        this->_uniqueuris.remove(urti);
                    }
                    if (this->_uniqueuris.contains(keyuu))
                    {
                        this->_uniqueuris.remove(keyuu);
                    }
                }
                else
                {
                    keyuu = this->_flyUrl;
                    urti = QString("toc") + QString::number(genKeyUrl(this->_flyUrl));
                }

                if (this->_uniqueuris.contains(keyuu))
                {
                    this->_uniqueuris.remove(keyuu);
                }
                EpubToc dax;
                dax.title = this->_flyUrl;
                dax.type = QLatin1String("toclist");
                dax.jumpUrl = urti;
                dax.zipUrl = keyuu;
                dax.reserve = this->_flyUrl;
                dax.orderid = idx + 2;
                dax.md843 = genKeyUrl(keyuu);
                this->_menuItem.prepend(dax);
                this->_minNrorder = qMin(this->_minNrorder, idx);
                this->_maxNrorder = qMax(this->_maxNrorder, idx);
            }
            else
            {
                EPUBDEBUG() << this->_flyName << " to int fail not insert...";
            }
            this->readMenu(child);
        }
        else if(QLatin1String("navLabel") == child.tagName())
        {
            this->readMenu(child);
        }
        else if(QLatin1String("text") == child.tagName())
        {
            this->_flyName = child.firstChild().toText().data();
        }
        else if (child.isText())
        {
            EPUBDEBUG() << "ERROR...!";
        }
        else if (QLatin1String("manifest") == child.tagName())
        {
            this->readMenu(child);
        }
        else if (QLatin1String("item") == child.tagName())
        {
            this->fileListRecord(child);
        }
        else
        {
            EPUBDEBUG() << "not know tag  :" << child.tagName();
        }
        child = child.nextSiblingElement();
    }
    return true;
}

auto Document::getPageName(const QString fileName, const QString tag) -> QDomNodeList
{
    EPUBDEBUG() << "Request GetPageName: file_name/tag" << fileName << " : " << tag << " current actioncycle.";

    QDomNodeList defineterror;
    QString tmp;
    for(auto&& it = this->_cache.begin(); it != this->_cache.end(); ++it)
    {
        if(it.key() == fileName)
        {
            QByteArray chunx = it.value();
            QDomElement e = this->make2DomElementXmlFile(chunx);
            tmp = e.tagName();
            QDomNodeList der = e.elementsByTagName(tag);
            QDomNodeList dera = e.elementsByTagName(e.tagName());

            if(der.count() > 0) return der;
            else if(dera.count() > 0) return dera;
        }
    }
    EPUBDEBUG() << "Request Error: " << fileName << ":" << tag << " export FAIL!....";
    return defineterror;
}

auto Document::fileListRecord(const QDomElement e) -> bool
{
    if(QLatin1String("item") != e.tagName()) return false;

    const QString xid = e.attribute("id");
    const QString xhref = e.attribute("href");
    const QString basexhref = this->_baseRefDir + e.attribute("href");
    const QString xmedia = e.attribute("media-type");

    if(xid.isEmpty())
    {
        this->setEpubError(QString("Not found attributes id in item tag from content.opf"));
    }

    if(xmedia.isEmpty())
    {
        this->setEpubError(QString("Not found attributes media-type in item tag from content.opf"));
    }
    if(xhref.isEmpty())
    {
        this->setEpubError(QString("Not found attributes href in item tag from content.opf"));
    }

    if(xmedia.contains(QLatin1String("image/")))
    {
        if(this->_imageData.contains(xhref))
        {
            this->_recImages << xhref;
        }
        else
        {
            if(this->_imageData.contains(basexhref))
            {
                this->_useBaseRef = true;
            }
            else
            {
                this->setEpubError(QString("Nofile: %2 from META-INF/container.xml:-> !!%1")
                            .arg(xhref)
                            .arg(this->_rootFollowFromFile));
            }
        }
    }
    else if(xmedia.contains(QLatin1String("application/xhtml")))
    {
        if(this->_cache.contains(xhref))
        {
            EpubToc dax;
            dax.title = QLatin1String("Unknow Title");
            dax.type = QLatin1String("xhtml");
            dax.jumpUrl = xid;
            dax.idref = xid;
            dax.md843 = genKeyUrl(xhref);
            dax.zipUrl = xhref;
            dax.reserve = xhref;
            dax.orderid = 0;
            this->_pageItem.append(dax);
        }
        else
        {
            if(this->_cache.contains(basexhref))
            {
                this->_useBaseRef = true;
                EpubToc dax;
                dax.title = QLatin1String("Unknow Title");
                dax.type = QLatin1String("xhtml");
                dax.jumpUrl = xid;
                dax.idref = xid;
                dax.md843 = genKeyUrl(basexhref);
                dax.zipUrl = basexhref;
                dax.reserve = xhref;
                dax.orderid = 0;
                this->_pageItem.append(dax);
            }
            else
            {
                this->setEpubError(QString("Nofile: %2 from META-INF/container.xml:-> !!%1")
                            .arg(xhref).arg(this->_rootFollowFromFile));
            }
        }
    }
    return true;
}

auto Document::saveFile(const QString& name, DataMap::iterator& data) -> void
{
    QString savePath = this->_bookPath + name;
    QString dirPath = savePath.left(savePath.lastIndexOf("/")) + "/";
    QDir dir(dirPath);
    if(not dir.exists()) dir.mkpath(dirPath);

    QFile file(savePath);
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(data.value());
    }
}

} // namespace EPUB