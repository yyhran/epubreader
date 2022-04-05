#ifndef __E_DOCUMENT__H
#define __E_DOCUMENT__H

#include "unzip.h"
#include <QCoreApplication>
#include <QTextCursor>

#if 0
#define EPUBDEBUG qDebug
#define EPUBWARNING qWarning
#define STAGE 1
#else
#define EPUBDEBUG if(0) qDebug
#define EPUBWARNING if(0) qWarning
#define STAGE 0
#endif

namespace EPUB {

struct EpubToc
{
    int order;
    QString text;
    QString src;
    QString upper;

    auto print() -> void
    {
        qDebug() << "order:" << order << '\n'
                 << "text: " << text << '\n'
                 << "upper: " << upper << '\n'
                 << "src: " << src << '\n';    
    }
};

class Document : public QTextDocument
{
public:
    using DataMap = QMap<QString, QByteArray>;
    using MetaInfo = QMap<QString, QStringList>;

    Document(const QString& fileName, QObject* parent = Q_NULLPTR);
    ~Document();

    auto opened() const -> bool { return this->_opened; }
    auto openedFile() const -> QString { return this->_openedFile; }
    auto setFile(const QString& file) -> void;
    auto getToc() const -> QList<EpubToc> { return this->_toc; }
    auto getMeta() const -> MetaInfo { return this->_metaInfo; }
    auto getFont() const -> QFont { return this->defaultFont(); }

private:
    auto open() -> bool;
    auto getDomElementFromXml(const QByteArray& xml, bool usenamespace = false) -> QDomElement;
    auto metaReader(QByteArray& xml) -> bool;
    auto readMenu(const QDomElement& element, const QString& text = "") -> bool;
    auto changePath(const QString& name, QByteArray& xml) -> void;
    auto getPageName(const QString fileName, const QString tag = "body") -> QDomNodeList;
    auto saveFile(const QString& name, QByteArray& data) -> void;

private:
    DataMap _imageData;
    DataMap _textData;
    DataMap _styleData;
    DataMap _metaData;
    MetaInfo _metaInfo;
    QList<EpubToc> _toc;
    QString _coverImage;
    QString _fileName;
    QString _bookTitle;
    QString _imgPath;
    QString _baseRefDir;
    QString _bookPath;
    QString _openedFile;
    bool _runOnRam;
    bool _opened;
};

} // namespace EPUB

#endif // __E_DOCUMENT__H