#include "htmlDriver.h"


HtmlDriver::HtmlDriver()
    : _fullHtmlNow("textutil -convert .. unable to read!")
    , _cacheOp(-1)
{
    _txtUtils = runWhich(QStringLiteral("textutil"));
    this->cleanCache();
}
HtmlDriver::~HtmlDriver()
{
    downDirRM(CACHEBUFFERDISKTMP);
}

auto HtmlDriver::cleanCache() -> void
{
    downDirRM(CACHEBUFFERDISKTMP);
    QDir dirSystem;
    if(dirSystem.mkpath(CACHEBUFFERDISKTMP))
    {
        this->_cacheOp = 1;
    }
}
auto HtmlDriver::refdToHtml(const QString& file) -> void
{
    this->cleanCache();
    this->diskTextUtils(file);
}
auto HtmlDriver::diskTextUtils(const QString& file) -> void
{
    if(isFile(CACHEFILETMP)) unLink(CACHEFILETMP);

    qDebug() << "### handler use-> " << __FUNCTION__ << file;
}
auto HtmlDriver::imageFind(QString& html) -> void
{

}
auto HtmlDriver::picEncode(QImage im) -> QByteArray
{

    return QByteArray();
}

auto filePutContents(const QString& fullFileName, QString& xml, int modus) -> bool
{

    return true;
}
auto readFile(const QString& fullFileName) -> QString
{
    QString inside{""};
    QFile file(fullFileName);
    if(file.exists())
    {
        if(file.open(QFile::ReadOnly | QFile::Text))
        {
            inside = QString(file.readAll().constData());
            file.close();
        }
    }
    return inside;
}
auto isTextTool() -> bool
{

    return false;
}
auto isFile(const QString& fullFileName) -> bool
{

    return false;
}
auto unLink(const QString& fullFileName) -> bool
{
    if(fullFileName.size() > 0)
    {
        QFile f(fullFileName);
        if(f.exists(fullFileName))
        {
            if(f.remove()) return true;
        }
    }
    return false;
}
auto copy(const QString& inputFile, const QString& outFile) -> bool
{

    return false;
}
auto downDirRM(const QString& d) -> void
{
    QDir dir(d);
    QDir enddir(d);
    if(dir.exists())
    {
        const QFileInfoList list = dir.entryInfoList();
        QFileInfo fi;
        for(int i{0}; i < list.size(); ++i)
        {
            fi = list.at(i);
            if(fi.isDir() and fi.fileName() != "." and fi.fileName() != "..")
            {
                downDirRM(fi.absoluteFilePath());
            }
            else if(fi.isFile())
            {
                unLink(fi.absoluteFilePath());
            }
        }
        dir.rmdir(d);
    }
}
auto encodeBase64(QString& xml) -> QString
{
    QByteArray ret;
    ret.append(xml);
    return ret.toBase64();
}
auto encodeBase64(QByteArray chunk) -> QString
{
    return chunk.toBase64();
}
auto decodeBase64(QString& xml) -> QString
{
    QByteArray xcode("");
    xcode.append(xml);
    QByteArray precode(QByteArray::fromBase64(xcode));
    QString ret = precode.data();
    return ret;
}
auto decodeBase64(QByteArray chunk) -> QString
{
    QByteArray precode(QByteArray::fromBase64(chunk));
    QString ret = precode.data();
    return ret;
}
auto isNumeric(QString& incomming) -> bool
{
    QString str = incomming;
    str.replace(QString(" "), QString(""));
    bool ok(false);
    str.toFloat(&ok);
    return ok;
}