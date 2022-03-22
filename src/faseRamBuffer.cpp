#include "fastRamBuffer.h"

Fastrambuffer::Fastrambuffer()
    : _x(new QBuffer())
{
    this->_x->open(QIODevice::ReadWrite);
}

auto Fastrambuffer::loadFile(const QString file) -> bool
{
    QMimeDatabase db;
    this->_mime = db.mimeTypeForFile(file).name();
    qDebug() << this->_mime << " mime incomming ram --- " << __LINE__;
    QFileInfo fix(file);
    this->_absolutePathFromFile = fix.absolutePath();
    this->_fileName = fix.fileName();
    this->_fcurrentSize = fix.size();
    if(QStringLiteral("rtfd") == fix.completeSuffix())
    {
        QByteArray xdax(this->_absolutePathFromFile.toUtf8());
        this->device()->write(xdax);
        this->_x->seek(0);
        return true;
    }
    qDebug() << this->_mime << " size: " << BytesRead(this->_fcurrentSize)
             << " -limit " << BytesRead(megaLimit)
             << ", mime incomming ram --- " << __LINE__;

    if(this->_fcurrentSize > megaLimit) return false;
    qDebug() << this->_mime << " mime incomming ram --- " << __LINE__;

    if(this->_mime.contains(".oasis."))
    {
        QByteArray signature(this->_absolutePathFromFile.toUtf8());
        signature.prepend(QByteArray("opendocument,calligra,okular,openoffice"));
        this->device()->write(signature);
        this->_x->seek(0);
        return true;
    }
    qDebug() << this->_mime << " mime incomming ram --- " << __LINE__;

    QFile f(file);
    if(f.exists())
    {
        if(f.open(QFile::ReadOnly))
        {
            this->_x->write(f.readAll());
            this->_x->seek(0);
            this->_scurrentSize = this->_x->bytesAvailable();
            return true;
        }
    }
    return false;
} 
auto Fastrambuffer::putOnFile(const QString file) -> bool
{
    if(file.contains("/", Qt::CaseInsensitive))
    {
        QString lastDir = file.left(file.lastIndexOf("/")) + "/";
        QDir dira(lastDir);
        if(not dira.mkpath(lastDir)) return false;

        QFile f(file);
        this->_x->seek(0);
        if(f.open(QFile::WriteOnly))
        {
            uint ret = f.write(this->stream());
            f.close();
            return ret > 0 ? true : false;
        }
    }
    return false;
}
auto Fastrambuffer::toHtmlSimulate() -> QString
{
    QStringList lines = this->stripTag(this->data()).split(QRegExp("[\n"), QString::KeepEmptyParts);
    QString str = lines.join("<br/>");
    str.prepend(QString("<p>"));
    str.append(QString("<p/>"));
    return str;
}
auto Fastrambuffer::stripTag(QString txt) -> QString
{
    if(txt.size() < 3) return QString();
    QString tmp;
    for(int i{0}; i < txt.size(); ++i)
    {
        QChar vox(txt.at(i));
        int letter{vox.unicode()};
        if(60 == letter || 62 == letter || 250 < letter)
        {
            tmp += QString("&#") + QString::number(letter) + QString(";");
        }
        else tmp.append(QString(vox));
    }
    return tmp;
}
auto Fastrambuffer::clear() -> bool
{
    this->_x->close();
    this->_x->seek(0);
    this->_x->write(QByteArray());
    return (0 == this->_x->bytesAvailable()) ? true : false;
}