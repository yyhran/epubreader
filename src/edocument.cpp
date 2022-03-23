#include "edocument.h"

namespace EPUB {

Document::Document(const QString& fileName, const QString& dir)
{

}
Document::~Document()
{

}

auto Document::open() -> bool
{

}
auto Document::setEpubError(QString& msg) -> void
{

}
auto Document::make2DomElementXmlFile(const QByteArray xml) -> QDomElement
{

}
auto Document::images() const -> QMap<QString, QByteArray>
{

}
auto Document::structure() const -> QMap<QString, QByteArray>
{

}
auto Document::fileGoTo(const QString fullFileName, QByteArray chunk) -> bool
{

}
auto Document::removeFromRam(const QString fileName) -> bool
{

}
auto Document::picEncodeCompressed(QImage im, bool press) -> QByteArray
{

}
auto Document::lostFoundSyncro() -> void
{

}
auto Document::pageBuilder() -> void
{

}
auto Document::getPageKeyMd843(const int idmd, EpubToc& item) -> void
{

}
auto Document::getMenuOrderID(const int x, EpubToc& item) -> void
{

}
auto Document::getPageOrderID(const QString ref, EpubToc& item) -> void
{

}
auto Document::metaReader() -> bool
{

}
auto Document::cacheFinder(const QString findFile) -> bool
{

}
auto Document::readMenu(const QDomElement& element) -> bool
{

}
auto Document::getPageName(const QString fileName, const QString tag) -> QDomNodeList
{

}
auto Document::fileListRecord(const QDomElement e) -> bool
{

}

}