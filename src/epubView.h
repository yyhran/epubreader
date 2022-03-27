#ifndef __EPUB_VIEW__H
#define __EPUB_VIEW__H

#include <QTextBrowser>
#include <QWidget>
#include <QApplication>
#include <QCloseEvent>
#include <QComboBox>
#include <QCoreApplication>
#include <QCursor>
#include <QDebug>
#include <QDesktopWidget>
#include <QDialog>
#include <QDir>
#include <QEvent>
#include <QFileDialog>
#include <QIcon>
#include <QKeySequence>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QObject>
#include <QPlainTextEdit>
#include <QPoint>
#include <QProgressBar>
#include <QPushButton>
#include <QResizeEvent>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QStyle>
#include <QSystemTrayIcon>
#include <QTextEdit>
#include <QTimer>
#include <QWidget>
#include <QWidgetAction>
#include <QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QGridLayout>
#include <QHeaderView>
#include <QDateTime>
#include <QTextBrowser>
#include <QDate>
#include <QPainter>

#include "edocument.h"

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

class EpubView : public QWidget 
{
    Q_OBJECT

public:
    explicit EpubView(QWidget* parent = 0);
    ~EpubView();

    auto loadFile(const QString& path) -> void;

protected:
    void paintEvent(QPaintEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;

private:
    auto scroll(int amount) -> void;
    auto scrollPage(int amount) -> void;

private:
    EPUB::Document* _document;
    int _offset;
};

#endif // __EPUB__VIEW__H