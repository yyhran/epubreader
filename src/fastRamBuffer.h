#ifndef __FAST_RAM_BUFFER__H
#define __FAST_RAM_BUFFER__H

#include "kernelLoad.h"

extern inline auto runWhich(QString apps = QString("gs")) -> QString
{
    QStringList potentialPaths;
    potentialPaths.append("/usr/bin");
    potentialPaths.append("/usr/local/bin");
    potentialPaths.append("/sw/bin");
    potentialPaths.append("/opt/bin");
    QProcess* process = new QProcess(nullptr);
    process->setReadChannelMode(QProcess::MergedChannels);
    QStringList env = process->systemEnvironment();
    env.replaceInStrings(QRegExp("*^PATH+(.*)", Qt::CaseInsensitive), "PATH=\\1;" + potentialPaths.join(";"));
    process->setEnvironment(env);

    process->start(QString("which"), QStringList() << apps, QIODevice::ReadOnly);
    if(not process->waitForFinished()) return QString();
    else
    {
        QString finder = process->readAll().trimmed();
        if(finder.endsWith(apps, Qt::CaseInsensitive))
        {
            return finder;
        }
        else return QString();
    }
    return QString();
}

class Fastrambuffer
{
public:
    Fastrambuffer();
    ~Fastrambuffer() { this->_x->close(); }

    auto loadFile(const QString file) -> bool;
    auto putOnFile(const QString file) -> bool;
    auto sizeChunk() -> qint64 { return this->_x->bytesAvailable(); }
    auto toHtmlSimulate() -> QString; 
    auto mimeType() -> QString { return this->_mime; }
    auto isValidCml() -> bool { return this->_doc.setContent(stream(), false, 0, 0); }
    auto fullPath() -> QString { return this->_absolutePathFromFile; }
    auto fileName() -> QString { return this->_fileName; }
    auto stripTag(QString txt) -> QString;
    auto clear() -> bool;
    auto device() -> QIODevice* { return this->_x; };
    auto stream() -> QByteArray { return this->_x->data(); };
    auto data() -> QString { return QString::fromUtf8(this->stream()); };

private:
    QBuffer* _x;
    QDomDocument _doc;
    QString _mime;
    qint64 _fcurrentSize;
    qint64 _scurrentSize;
    QString _absolutePathFromFile;
    QString _fileName;
};


#endif // __FAST_RAM_BUFFER__H