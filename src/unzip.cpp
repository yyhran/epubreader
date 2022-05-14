#include "unzip.h"

// local header size
static constexpr uint UNZIP_LOCAL_HEADER_SIZE = 26;
// central directory file entry size
static constexpr uint UNZIP_CD_ENTRY_SIZE_NS = 42;
// data descriptor size
static constexpr uint UNZIP_DD_SIZE = 12;
// end of central directory size
static constexpr uint UNZIP_EOCD_SIZE = 22;
// local header entry encryption header size
static constexpr uint UNZIP_LOCAL_ENC_HEADER_SIZE = 12;

// some offsets inside a CD record
static constexpr uint UNZIP_CD_OFF_VERSION_MADE = 0;
static constexpr uint UNZIP_CD_OFF_VERSION = 2;
static constexpr uint UNZIP_CD_OFF_GPFLAG = 4;
static constexpr uint UNZIP_CD_OFF_CMETHOD = 6;
static constexpr uint UNZIP_CD_OFF_MODT = 8;
static constexpr uint UNZIP_CD_OFF_MODD = 10;
static constexpr uint UNZIP_CD_OFF_CRC32 = 12;
static constexpr uint UNZIP_CD_OFF_CSIZE = 16;
static constexpr uint UNZIP_CD_OFF_USIZE = 20;
static constexpr uint UNZIP_CD_OFF_NAMELEN = 24;
static constexpr uint UNZIP_CD_OFF_XLEN = 26;
static constexpr uint UNZIP_CD_OFF_COMMLEN = 28;
static constexpr uint UNZIP_CD_OFF_LHOFFSET = 38;

// some offsets inside a local header record
static constexpr uint UNZIP_LH_OFF_VERSION = 0;
static constexpr uint UNZIP_LH_OFF_GPFLAG = 2;
static constexpr uint UNZIP_LH_OFF_CMETHOD = 4;
static constexpr uint UNZIP_LH_OFF_MODT = 6;
static constexpr uint UNZIP_LH_OFF_MODD = 8;
static constexpr uint UNZIP_LH_OFF_CRC32 = 10;
static constexpr uint UNZIP_LH_OFF_CSIZE = 14;
static constexpr uint UNZIP_LH_OFF_USIZE = 18;
static constexpr uint UNZIP_LH_OFF_NAMELEN = 22;
static constexpr uint UNZIP_LH_OFF_XLEN = 24;

// some offsets inside a data descriptor record
static constexpr uint UNZIP_DD_OFF_CRC32 = 0;
static constexpr uint UNZIP_DD_OFF_CSIZE = 4;
static constexpr uint UNZIP_DD_OFF_USIZE = 8;

// some offsets inside a EOCD record
static constexpr uint UNZIP_EOCD_OFF_ENTRIES = 6;
static constexpr uint UNZIP_EOCD_OFF_CDOFF = 12;
static constexpr uint UNZIP_EOCD_OFF_COMMLEN = 16;

// max version handled by this
static constexpr uint UNZIP_VERSION = 0x14;
static constexpr uint ZIP_VERSION = 20;

static inline auto readUInt(const uchar* data) -> uint
{
    return static_cast<uint>((data[0]) + (data[1] << 8) + (data[2] << 16) + (data[3] << 24));
}

static inline auto readUShort(const uchar* data) -> ushort
{
    return static_cast<ushort>((data[0]) + (data[1] << 8));
}

static auto inflate(Bytef *dest, ulong* destLen, const Bytef* source, ulong sourceLen) -> int
{
    z_stream stream;
    int err{0};

    stream.next_in = const_cast<Bytef*>(source);
    stream.avail_in = static_cast<uInt>(sourceLen);
    if(static_cast<uLong>(stream.avail_in) != sourceLen) return Z_BUF_ERROR;

    stream.next_out = dest;
    stream.avail_out = static_cast<uInt>(*destLen);
    if(static_cast<uLong>(stream.avail_out) != *destLen) return Z_BUF_ERROR;

    stream.zalloc = static_cast<alloc_func>(0);
    stream.zfree = static_cast<free_func>(0);

    err = inflateInit2(&stream, -MAX_WBITS);
    if(Z_OK != err) return err;

    err = inflate(&stream, Z_FINISH);
    if(Z_STREAM_END != err)
    {
        inflateEnd(&stream);
        if(Z_NEED_DICT == err
        or (Z_BUF_ERROR == err and 0 == stream.avail_in))
        return Z_DATA_ERROR;
        return err;
    }
    *destLen = stream.total_out;

    err = inflateEnd(&stream);
    return err;
}

UnZipStream::UnZipStream(const QString& odtfile)
    : d_(new QBuffer)
    , isOpen_(false)    
{
    ErrorCode ec = ReadFailed;
    this->uBuffer = reinterpret_cast<unsigned char*>(this->buffer1);
    if(this->d_->open(QIODevice::ReadWrite))
    {
        this->isOpen_ = this->LoadFile(odtfile);
        ec = this->seekToCentralDirectory();
        UNZIPDEBUG() << "error: " << ec;
        if(UnZipStream::OkFunky != ec)
        {
            this->isOpen_ = false;
        }
        if(0 == this->cdEntryCount)
        {
            this->isOpen_ = false;
        }
    }

    if(not this->isOpen_)
    {
        this->clear();
    }
    else
    {
        ec = this->openArchive();
        if(UnZipStream::OkFunky == ec)
        {
            QStringList pieces;
            for(int i{0}; i < fileHeaders.size(); ++i)
            {
                FileHeader metaheader = this->fileHeaders.at(i);
                const QString zfile = QString::fromLocal8Bit(metaheader.fileName);
                if(zfile.indexOf("/"))
                {
                    QStringList subdir = zfile.split("/");
                    pieces << subdir;
                }
                QByteArray chunk = fileByte(zfile);
                this->zipFiles << zfile;
                if(chunk.size() > 0)
                {
                    this->coreFileList_.insert(zfile, chunk);
                    UNZIPDEBUG() << "content size() = " << chunk.size();
                }
            }
            this->zipFiles << QString("##items##");
            this->zipFiles << pieces;
            this->zipFiles.removeDuplicates();
        }
    }
}

UnZipStream::~UnZipStream()
{
    this->clear();
    delete this->d_;
    this->d_ = Q_NULLPTR;
}

auto UnZipStream::fileByte(const QString& fileName) -> QByteArray
{
    int compressedSize{0};
    int uncompressedSize{0};
    bool found{false};
    int i{0};
    for(; i < this->fileHeaders.size(); ++i)
    {
        if(fileName == QString::fromLocal8Bit(this->fileHeaders.at(i).fileName))
        {
            found = true;
            break;
        }
    }
    this->start(); // seek 0
    if(not found  or not this->device()) return QByteArray();

    FileHeader metaheader = this->fileHeaders.at(i);
    compressedSize = readUInt(metaheader.h.compressedSize);
    uncompressedSize = readUInt(metaheader.h.uncompressedSize);
    if(uncompressedSize < 1) return QByteArray();

    int start{static_cast<int>(readUInt(metaheader.h.offsetLocalHeader))};
    this->device()->seek(start);
    LocalFileHeader lh;
    this->device()->read(reinterpret_cast<char*>(&lh), sizeof(LocalFileHeader));
    uint skip{static_cast<uint>(readUShort(lh.fileNameLength) + readUShort(lh.extraFieldLength))};
    this->device()->seek(this->device()->pos() + skip);
    int compressionMethod{readUShort(lh.compressionMethod)};
    QByteArray compressed = this->device()->read(compressedSize);
    const QString zfile = QString::fromLocal8Bit(metaheader.fileName);
    UNZIPDEBUG() << "||| " << compressionMethod
                 << " ||| fileByte file" << zfile
                 << " s: " << uncompressedSize
                 << ": " << compressedSize;

    if(0 == compressionMethod)
    {
        // no compression
        compressed.truncate(uncompressedSize);
        return compressed;
    }
    else if(8 == compressionMethod)
    {
        // real unzip part file
        compressed.truncate(compressedSize);
        QByteArray decompressChunk;
        ulong len{static_cast<ulong>(qMax(uncompressedSize, 1))};
        int res{0};
        do
        {
            decompressChunk.resize(len);
            res = inflate(reinterpret_cast<uchar*>(decompressChunk.data()), &len,
                          reinterpret_cast<uchar*>(const_cast<char*>(compressed.constData())), compressedSize);
            if(Z_OK == res)
            {
                if(static_cast<int>(len) != decompressChunk.size())
                {
                    decompressChunk.resize(len);
                }
                break;
            }
            else
            {
                decompressChunk.clear();
                UNZIPWARNING("UnZip: Z_DATA_ERROR: Input data is corrupted!");
            }
        } while (Z_BUF_ERROR == res);
        return decompressChunk;
    }
    UNZIPWARNING() << "UnZip: Unkonw compression method!";
    return QByteArray();
}

auto UnZipStream::openArchive() -> UnZipStream::ErrorCode
{
    Q_ASSERT(this->device());
    if(not this->canRead())
    {
        UNZIPDEBUG() << "Unable to open device for reading...";
        return UnZipStream::OpenFailed;
    }

    this->start();

    uchar tmp[4];
    this->device()->read(reinterpret_cast<char*>(tmp), 4);
    if(0x04034b50 != this->getULong(tmp, 0))
    {
        UNZIPWARNING() << "UnZip: not a zip file!";
        return UnZipStream::OpenFailed;
    }

    // find end of directory header
    int i{0};
    int startOfDirectory{-1};
    int numDirEntries{0};
    EndOfDirectory eod;
    while(-1 == startOfDirectory)
    {
        int pos{static_cast<int>(this->device()->size() - sizeof(EndOfDirectory) - i)};
        if(pos < 0 or i > 65535)
        {
            UNZIPWARNING() << "UnZip: end of directory not found!";
            return UnZipStream::OpenFailed;
        }
        this->device()->seek(pos);
        this->device()->read(reinterpret_cast<char*>(&eod), sizeof(EndOfDirectory));
        if(0x06054b50 == readUInt(eod.signature)) break;
        ++i;
    }
    startOfDirectory = readUInt(eod.dirStartOffset);
    numDirEntries = readUShort(eod.numDirEntries);
    if(this->cdEntryCount != numDirEntries) return UnZipStream::OpenFailed;

    UNZIPDEBUG("start of directory at %d, num dir entries = %d", startOfDirectory, numDirEntries);
    int commentLength{readUShort(eod.commentLength)};
    if(commentLength != i)
    {
        UNZIPWARNING() << "UnZip: failed to parse zip file!";
        return UnZipStream::OpenFailed;
    }
    this->commentario = this->device()->read(qMin(commentLength, i));
    this->device()->seek(startOfDirectory);
    for(i = 0; i < numDirEntries; ++i)
    {
        FileHeader header;
        int read{static_cast<int>(this->device()->read(reinterpret_cast<char*>(&header.h), sizeof(GentralFileHeader)))};
        if(read < static_cast<int>(sizeof(GentralFileHeader)))
        {
            UNZIPWARNING() << "UnZip: Failed to read complete header, index may be incomplete!";
            break;
        }
        if(0x02014b50 != readUInt(header.h.signature))
        {
            UNZIPWARNING() << "UnZip: incalid header signature, index may be incomplete!";
            break;
        }

        int l{readUShort(header.h.fileNameLength)};
        header.fileName = this->device()->read(l);
        if(header.fileName.length() != l)
        {
            UNZIPWARNING() << "UnZip: Failed to read filename from zip index, index may be incomplete!";
            break;
        }
        l = readUShort(header.h.extraFieldLength);
        header.extraField = this->device()->read(l);
        if(header.extraField.length() != l)
        {
            UNZIPWARNING() << "UnZip: Failed to read extra field in zip file, skipping file, index may be incomplete!";
            break;
        }
        l = readUShort(header.h.fileCommentLength);
        header.fileComment = this->device()->read(l);
        if(header.fileComment.length() != l)
        {
            UNZIPWARNING() << "UnZip: Failed to read file comment, index may be incomplete!";
            break;
        }
        this->fileHeaders.append(header);
    }
    return UnZipStream::OkFunky;
}

auto UnZipStream::seekToCentralDirectory() -> UnZipStream::ErrorCode
{
    Q_ASSERT(this->device());

    qint64 length = this->device()->size();
    qint64 offset = length - UNZIP_EOCD_SIZE;
    if(UNZIP_EOCD_SIZE > length) return UnZipStream::InvalidArchive;
    if(not this->device()->seek(offset)) return UnZipStream::SeekFailed;
    if(UNZIP_EOCD_SIZE != this->device()->read(this->buffer1, UNZIP_EOCD_SIZE)) return UnZipStream::ReadFailed;
    bool eocdFound{'P' == this->buffer1[0] && 'K' == this->buffer1[1]
                   && 0x05 == this->buffer1[2] && 0x06 == this->buffer1[3]};
    if(eocdFound) this->eocdOffset = offset;
    else return UnZipStream::HandleCommentHere;

    if(not eocdFound) return UnZipStream::InvalidArchive;
    offset = this->getULong(reinterpret_cast<const unsigned char*>(this->buffer1),
                            UNZIP_EOCD_OFF_CDOFF + 4);
    this->cdOffset = offset;
    this->cdEntryCount = this->getUShort(reinterpret_cast<const unsigned char*>(this->buffer1),
                                         UNZIP_EOCD_OFF_ENTRIES +4);
    quint16 commentLength = this->getUShort(reinterpret_cast<const unsigned char*>(this->buffer1),
                                            UNZIP_EOCD_OFF_COMMLEN + 4);
    if(0 != commentLength) return UnZipStream::HandleCommentHere;
    if(not this->device()->seek(this->cdOffset)) return UnZipStream::SeekFailed;

    return UnZipStream::OkFunky;
}

auto UnZipStream::LoadFile(const QString& file) -> bool
{
    if(this->clear())
    {
        QFile f(file);
        if(f.exists())
        {
            if(f.open(QFile::ReadOnly))
            {
                this->d_->write(f.readAll());
                f.close();
                this->start();
                return true;
            }
        }
    }
    return false;
}

auto UnZipStream::getULong(const unsigned char* data, quint32 offset) -> quint32
{
    quint32 res{static_cast<quint32>(data[offset])};
    res |= (static_cast<quint32>(data[offset + 1]) << 8);
    res |= (static_cast<quint32>(data[offset + 2]) << 16);
    res |= (static_cast<quint32>(data[offset + 3]) << 24);
    return res;
}

auto UnZipStream::getULLong(const unsigned char* data, quint32 offset) -> quint64
{
    quint64 res{static_cast<quint64>(data[offset])};
    res |= (static_cast<quint64>(data[offset + 1]) << 8);
    res |= (static_cast<quint64>(data[offset + 2]) << 16);
    res |= (static_cast<quint64>(data[offset + 3]) << 24);
    res |= (static_cast<quint64>(data[offset + 1]) << 32);
    res |= (static_cast<quint64>(data[offset + 2]) << 40);
    res |= (static_cast<quint64>(data[offset + 3]) << 48);
    res |= (static_cast<quint64>(data[offset + 3]) << 56);
    return res;
}

auto UnZipStream::getUShort(const unsigned char* data, quint32 offset) -> quint16
{
    return static_cast<quint16>(data[offset]) | (static_cast<quint16>(data[offset + 1]) << 8);
}

auto UnZipStream::clear() -> bool
{
    this->d_->write(QByteArray());
    this->start();
    return this->d_->bytesAvailable() == 0 ? true : false;
}
