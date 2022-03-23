#ifndef __HTML_DRIVER__H
#define __HTML_DEIVER__H

#include "kernelLoad.h"
#include "fastRamBuffer.h"

auto filePutContents(const QString& fullFileName, QString& xml, int modus = 1) -> bool;
auto readFile(const QString& fullFileName) -> QString;
auto isTextTool() -> bool;
auto isFile(const QString& fullFileName) -> bool;
auto unLink(const QString& fullFileName) -> bool;
auto copy(const QString& inputFile, const QString& outFile) -> bool;
auto downDirRM(const QString& d) -> void;
// encode to name pics file ...
auto encodeBase64(QString& xml) -> QString;
auto encodeBase64(QByteArray chunk) -> QString;
// decode to name pics file ...
auto decodeBase64(QString& xml) -> QString;
auto decodeBase64(QByteArray chunk) -> QString;
// check if a int or float numer
auto isNumeric(QString& incomming) -> bool;

#define __ONEPIXELGIF__ \
    QString("data:image/gif;base64,R0lGODlhAQABAAAAACH5BAEKAAEALAAAAAABAAEAAAICTAEAOw==")
#define CACHEBUFFERDISKTMP \
    QString("%1/.ernelversion").arg(QDir::homePath())
#define CACHEFILETMP \
    QString("%1/index.html").arg(CACHEBUFFERDISKTMP)

class HtmlDriver
{
public:
    HtmlDriver();
    ~HtmlDriver();

    auto isTextTool() -> bool { return (this->_txtUtils.size() > 4) ? true : false; };
    auto cleanCache() -> void;
    auto refdToHtml(const QString& file) -> void;
    auto diskTextUtils(const QString& file) -> void;
    auto resultsHtmlgz() -> QByteArray
    {
        qDebug() << _HTMLCOMPRESSED.size() << "HtmlDriver return size!";
        return _HTMLCOMPRESSED;
    }

private:
    auto imageFind(QString& html) -> void;
    auto picEncode(QImage im) -> QByteArray;

private:
    QString _fullHtmlNow;
    QString _txtUtils;
    int _cacheOp;
    QByteArray _HTMLCOMPRESSED;
};

#endif // __HTML_DRIVER__H