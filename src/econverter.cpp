#include "econverter.h"

namespace EPUB {

Converter::Converter() { }

Converter::~Converter() { }

auto Converter::convert(const QString& fileName, const QString& dir) -> const QList<EpubToc>
{
    Document document(fileName, dir);
    if(not document.open()) return QList<EpubToc>();

    return document.menuList();
}

} // namespace EPUB