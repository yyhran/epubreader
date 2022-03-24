#ifndef __E_CONERTER__H
#define __E_CONERTER__H

#include <QObject>
#include <QTextCharFormat>
#include <QDOmDocument>
#include "edocument.h"

namespace EPUB {

class Converter
{
public:
    Converter();
    virtual ~Converter();

    virtual auto convert(const QString& fileName, const QString& dir) -> const QList<EpubToc>;
};

} // namespace EPUB
#endif // __E_CONERTER__H