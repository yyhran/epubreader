#ifndef __KERNEL_LOAD__H
#define __KERNEL_LOAD__H

#include <QAbstractTextDocumentLayout>
#include <QBuffer>
#include <QDebug>
#include <QDomDocument>
#include <QMarginsF>
#include <QMimeDatabase>
#include <QMimeType>
#include <QObject>
#include <QPainter>
#include <QTextDocument>
#include <QTextFrame>
#include <QTextFrameFormat>
#include <QXmlQuery>
#include <cmath>
#include <typeinfo>
#include <QDir>
#include <QtDebug>
#include <QDebug>
#include <QProcess>
#include <QMap>

#define POINT_TO_CM(cm) ((cm) / 28.3465058)
#define POINT_TO_MM(mm) ((mm) / 2.83465058)
#define MM_TO_POINT(mm) ((mm) * 2.83465058)
#define CM_TO_POINT(cm) ((cm) * 28.3465058)

#define volumega(x) ((x) / 1048576)

#define _SLIDERBGCOLOR_ QCorlor("#ebe9ed")
#define _EDITORBGCOLOR QCorlor("#ebe9ed")
#define _SHADOWCOLOR_ QCorlor("#d3d3d3")

static inline auto BytesRead(const qint64 size) -> QString
{
    if(size < 0) return QString();
    else if(size < 1024) return QObject::tr("%1 B").arg(QString::number(static_cast<double>(size), 'f', 0));
    else if(size < 1048576) return QObject::tr("%1 KB").arg(QString::number(static_cast<double>(size) / 1024, 'f', 0));
    else if(size < 1073741824) return QObject::tr("%1 MB").arg(QString::number(static_cast<double>(size) / 1048576, 'f', 2));
    else return QObject::tr("%1 GB").arg(QString::number(static_cast<double>(size) / 1073748124, 'f', 2));
    return QString();
}

#endif // __KERNEL_LOAD__H
