#ifndef __UNZIP__H
#define __UNZIP__H

#include <QFile>
#include <QString>
#include <QMap>
#include <QDebug>
#include <QBuffer>
#include <QIODevice>
#include <QByteArray>
#include <QStringList>
#include <QDomDocument>
#include <QTextDocument>
#include <QImage>
#include <QXmlSimpleReader>
#include <QMimeDatabase>
#include <QSet>
#include <Qdatetime>
#include <qendian.h>
#include <QDir>
#include <QFileInfo>
#include <QSharedData>
#include <zlib.h>
#include <qplatformdefs.h>
#include <QColor>
#include <QCryptographicHash>

#define UNZIP_READ_BUFFER (256 * 1024)

#if 1
#define UNZIPDEBUG qDebug
#define UNZIPWARNING qWarning
#else
#define UNZIPDEBUG if(0) qDebug
#define UNZIPWARNING if(0) qWarning
#endif

static inline QString MimeinFile(const QString file)
{
    QMimeDatabase db;
    return db.mimeTypeForFile(file).name();
}


struct LocalFileHeader
{
    uchar signature[4];
    uchar versionNeeded[2];
    uchar generalPurposeBits[2];
    uchar compressionMethod[2];
    uchar lastModFile[4];
    uchar crc32[4];
    uchar compressedSize[4];
    uchar uncompressedSize[4];
    uchar fileNameLength[2];
    uchar extraFieldLength[2];
};

struct DataDescriptor 
{
    uchar crc32[4];
    uchar compressedSize[4];
    uchar uncompressedSize[4];
};

struct GentralFileHeader
{
    uchar signature[4];
    uchar versionMade[2];
    uchar versionNeeded[2];
    uchar generalPurposeBits[2];
    uchar compressionMethod[2];
    uchar lastModFile[4];
    uchar crc32[4];
    uchar compressedSize[4];
    uchar uncompressedSize[4];
    uchar fileNameLength[2];
    uchar extraFieldLength[2];
    uchar fileCommentLength[2];
    uchar diskStart[2];
    uchar internalFileAttributes[2];
    uchar externalFileAttributes[4];
    uchar offsetLocalHeader[4];
    LocalFileHeader toLocalHeader() const;
};

struct EndOfDirectory
{
    uchar signature[4];
    uchar thisDisk[2];
    uchar startOfDirectoryDisk[2];
    uchar numDirEntriesThisDisk[2];
    uchar numDirEntries[2];
    uchar directorySize[4];
    uchar dirStartOffset[4];
    uchar commentLength[2];
};

struct FileHeader
{
    GentralFileHeader h;
    QByteArray fileName;
    QByteArray extraField;
    QByteArray fileComment;
};

class UnZipStream
{
public:
    using CoreFileList = QMap<QString, QByteArray>;

    UnZipStream(const QString& odtfile);
    ~UnZipStream();

    QByteArray fileByte(const QString &fileName);

    enum ErrorCode
    {
        OkFunky = 1000,
        ZlibInit,
        ZlibError,
        OpenFailed,
        PartiallyCorrupted,
        Corrupted,
        WrongPassword,
        NoOpenArchive,
        FileNotFound,
        ReadFailed,
        WriteFailed,
        SeekFailed,
        HandleCommentHere,
        CreateDirFailed,
        InvalidDevice,
        InvalidArchive,
        HeaderComsistencyError,
        Skip,
        SkipAll
    };

    auto device() const ->QIODevice* { return static_cast<QIODevice*>(this->_d);}
    auto stream() const -> QByteArray { return this->_d->data(); }
    auto canRead() const -> bool { return this->_isOpen; }
    auto fileList() const -> QStringList { return this->zipFiles; }
    auto listData() const -> CoreFileList { return this->_coreFileList; }

protected:
    char buffer1[UNZIP_READ_BUFFER];    
    char buffer2[UNZIP_READ_BUFFER];    
    QList<FileHeader> fileHeaders;
    QStringList zipFiles;
    QByteArray commentario;
    uint startOfDirectory;

    unsigned char* uBuffer;
    quint32 cdOffset;
    quint32 eocdOffset;
    quint16 cdEntryCount;

private:
    auto seekToCentralDirectory() -> ErrorCode;    
    auto openArchive() -> ErrorCode;
    auto getULong(const unsigned char* data, quint32 offset) -> quint32;
    auto getULLong(const unsigned char* data, quint32 offset) -> quint64;
    auto getUShort(const unsigned char* data, quint32 offset) -> quint16;

    auto clear() -> bool;
    auto LoadFile(const QString& file) -> bool;
    auto start() -> void { this->_d->seek(0);}

private:
    CoreFileList _coreFileList;
    QBuffer* _d;
    bool _isOpen;
};

#endif // __UNZIP__H
