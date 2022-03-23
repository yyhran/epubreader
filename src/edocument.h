#ifndef __E_DOCUMENT__H
#define __E_DOCUMENT__H

#include "unzip.h"

#define refill QObject::tr
#define recit2 QString::toStdString

#if 0
#define EPUBDEBUG qDebug
#define STAGE 1
#else
#define EPUBDEBUG if (0) qDebug
#define STAGE 0
#endif

struct EpubToc
{
    auto print() -> QString
    {
        QString ret = QString("<div><a href='#%1'>").arg(jumpUrl)
                    + QString("<h1>") + title + QString("</h1>")
                    + QString("</a></div>\n<!-- Url intern = (%1) ,%2, -->").arg(qurl()).arg(md843);
        return ret;
    }
    auto qurl() -> QString
    {
        QString ret = zipUrl;
        if (ret.isEmpty())
        {
            ret = reserve;
        }
        return ret;
    }

    auto debug() -> QString
    {
        QString ret = QString("|reserve:%1").arg(reserve);
        ret += QString("|idref:%1").arg(idref);
        ret += QString("|jumpurl:%1").arg(jumpUrl);
        ret += QString("|zipurl:%1").arg(zipUrl);
        ret += QString("|md843:%1").arg(md843);
        ret += QString("|orderid:%1").arg(orderid);
        return ret;
    }

    QString reserve; //// filename dest normal!!
    QString jumpUrl; //// toc meta id && page link!!
    QString zipUrl;
    QString title;
    QString type;
    QString idref;  /// only if content.opf
    int md843;
    int orderid;
};

namespace EPUB
{
class Document
{
public:
    Document(const QString& fileName, const QString& dir);
    ~Document();

    auto open() -> bool;
    auto menuList() const -> QList<EpubToc> { return this->_revisionPageItem; }
    auto lastErrorString() -> QString { return this->_recError.join(" ; "); }

private:
    auto setEpubError(QString& msg) -> void;
    auto make2DomElementXmlFile(const QByteArray xml) -> QDomElement;
    auto images() const -> QMap<QString, QByteArray>;
    auto structure() const -> QMap<QString, QByteArray>;
    auto fileGoTo(const QString fullFileName, QByteArray chunk) -> bool;
    auto removeFromRam(const QString fileName) -> bool;
    auto picEncodeCompressed(QImage im, bool press) -> QByteArray;
    auto lostFoundSyncro() -> void;
    auto pageBuilder() -> void;
    auto getPageKeyMd843(const int idmd, EpubToc& item) -> void;
    auto getMenuOrderID(const int x, EpubToc& item) -> void;
    auto getPageOrderID(const QString ref, EpubToc& item) -> void;
    auto metaReader() -> bool;
    auto cacheFinder(const QString findFile) -> bool;
    auto readMenu(const QDomElement& element) -> bool;
    auto getPageName(const QString fileName, const QString tag = "body") -> QDomNodeList;
    auto fileListRecord(const QDomElement e) -> bool;

private:
    QStringList _recError;
    QStringList _recImages;
    QStringList _rspine;
    QSet<QString> _uniqueuris;
    QMap<QString, QByteArray> _images;
    QMap<QString, QByteArray> _cache;
    QList<EpubToc> _menuItem;
    QList<EpubToc> _pageItem;
    QList<EpubToc> _revisionPageItem;
    QString _fileName;
    QString _fileTitle;
    QByteArray _eContent;
    QString _errorString;
    QString _flyID;
    QString _flyOrder;
    QString _flyUrl;
    QString _flyName;
    QString _coverPager;
    QString _startIndexPager;
    QString _rootFollowFromFile;
    QString _baseRefDir;
    QString _nextFileToReadCrono;
    QString _dirBrowserBook;
    QByteArray _nextFileToReadXmlChunk;
    bool _useBaseRef;
    int _summerror;
    int _minNrorder;
    int _maxNrorder;
    bool _showSynrorun;
    bool _compressOnRam;
};

}

#endif // __E_DOCUMENT__H