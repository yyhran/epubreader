#ifndef __E_DOCUMENT__H
#define __E_DOCUMENT__H

#include "unzip.h"
#include <QCoreApplication>

#define refill QObject::tr
#define recit2 QString::toStdString

#if 0
#define EPUBDEBUG qDebug
#define EPUBWARNING qWarning
#define STAGE 1
#else
#define EPUBDEBUG if(0) qDebug
#define EPUBWARNING if(0) qWarning
#define STAGE 0
#endif

#define METAINFOCONTAINERFILE QLatin1String("META-INF/container.xml")
#define CONTENENTOPFFILE QLatin1String("content.opf")

namespace EPUB {

class Document : public QTextDocument
{
public:
    using DataMap = QMap<QString, QByteArray>;
    using DataMapIterator = QMapIterator<QString, QByteArray>;

    Document(const QString& fileName, QObject* parent = Q_NULLPTR);
    ~Document();

    auto open() -> bool;
    auto opened() const -> bool { return this->_opened; }

private:
    auto getDomElementFromXml(const QByteArray& xml) -> QDomElement;
    auto metaReader(QByteArray& xml) -> bool;
    auto readMenu(const QDomElement& element) -> bool;
    auto getPageName(const QString fileName, const QString tag = "body") -> QDomNodeList;
    auto saveFile(const QString& name, DataMap::iterator& data) -> void;

private:
    QStringList _recImages;
    QStringList _rspine;
    QSet<QString> _uniqueuris;
    DataMap _cache;

    DataMap _imageData;
    DataMap _textData;
    DataMap _styleData;
    DataMap _metaData;
    QMap<QString, QStringList> _metaInfo;

    QString _fileName;
    QString _bookTitle;
    QString _coverPager;
    QString _baseRefDir;
    QString _bookPath;
    bool _useBaseRef;
    bool _opened;
};

} // namespace EPUB

#endif // __E_DOCUMENT__H