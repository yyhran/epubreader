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

    auto opened() const -> bool { return this->opened_; }
    auto openedFile() const -> QString { return this->openedFile_; }
    auto setFile(const QString& file) -> void;
    auto getToc() const -> QList<EpubToc> { return this->toc_; }
    auto getMeta() const -> MetaInfo { return this->metaInfo_; }
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
    DataMap imageData_;
    DataMap textData_;
    DataMap styleData_;
    DataMap metaData_;
    MetaInfo metaInfo_;
    QList<EpubToc> toc_;
    QString coverImage_;
    QString fileName_;
    QString bookTitle_;
    QString imgPath_;
    QString baseRefDir_;
    QString bookPath_;
    QString openedFile_;
    bool runOnRam_;
    bool opened_;
};

} // namespace EPUB

#endif // __E_DOCUMENT__H