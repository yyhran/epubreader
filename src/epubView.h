#ifndef __EPUB_VIEW__H
#define __EPUB_VIEW__H

#include <QTextBrowser>
#include <QWidget>

struct VoiceDocument
{
    auto print() -> QString
    {
        QString ret = QString("<div><a herf='#%1'>").arg(jumpUrl)
                    + QString("<h1>") + title + QString("</h1>")
                    + QString("</a></div>\n<!-- UnixTime/OrderID = (%1), %2 -->").arg(unixiTime).arg(orderID);
        return ret;
    }

    auto qurl() -> QString { return jumpUrl; }

    auto debug() -> QString
    {
        QString ret = QString("|text: %1").arg(text.size());
        ret += QString("|jumpurl: %1").arg(jumpUrl);
        ret += QString("|title: %1").arg(title);
        ret += QString("|lang: %1").arg(lang);
        ret += QString("|unixtime: %1").arg(unixiTime);
        ret += QString("|orderid: %1").arg(orderID);
        return ret;
    }

    QString text;
    QString jumpUrl;
    QString title;
    QString  lang;
    uint unixiTime;
    int readOK;
    int readStop;
    int orderID;
    bool local;
};

class EpubView : public QTextBrowser
{
    Q_OBJECT

public:
    explicit EpubView(QWidget* parent = 0);
    ~EpubView();
};

#endif // __EPUB__VIEW__H