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

#include "econverter.h"

#define WEBAGENTNAME "Google"
#define _PROGRAM_NAME_ QString("KernelApp")
#define _PROGRAM_NAME_DOMAINE_ QString("qt.io")
#define _ORGANIZATION_NAME_ QString("Avasoft")
#define _PROGRAM_VERSION_ QString("version 0.2.1 / go git")
#define _PROGRAM_OWNER_ QString("version 0.2.1")
#define _PROGRAM_TITLE_ QString("TestApp")
#define _PROGRAM_SHORT_NAME QString("docsrc_test")
#define TUMBNAIL_SIZE 120
#define ICONSW_SIZE 50
// #define _ZIPCACHEBOOK_ QString("%1/ebooks/fromlocal/").arg(QDir::homePath())
#define _ZIPCACHEBOOK_ QString("d:/GitHub/epubreader/build/ebooks/fromlocal/")
#define _FIRSTWINDOWTITLESHOW_ QString("Blabbering Ebooks & Document")
#define _TESTBOOKS_ QString("%1/tmp/").arg(_ZIPCACHEBOOK_)
#define core_application (static_cast<RDoc *>(QCoreApplication::instance()))

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

    auto readerMenu() -> QList<EpubToc> { return this->_activeEpub; }
    auto plainText() -> QString { return this->_text2Voice; }
    auto currentDoc() -> VoiceDocument { return this->_vcurrent; }
    auto weneedTxt() -> void;

signals:
    void paintMenuEpub(bool);
    void htmlReady();
    void vdocReady();
    void incomHtml();

public slots:
    void playFile(const QUrl localRemote);
    void openEpub(const QUrl urie);
    void jumpEpub(const QUrl urie);
    void startHtmlGrab(bool ok);
    void handleHtml(QString sHtml);

protected:
    auto rollContext(const QPoint& pos) -> void;
    auto centextMenuEvent(QContextMenuEvent* event);

protected:
    QString _htmlSource;
    QString _text2Voice;
    QString _doctitle;

private:
    int _moduNow;
    QSet<QString> _uniqueVdoc;
    QList<VoiceDocument> _voiceHistory;
    QList<EpubToc> _activeEpub;
    EPUB::Converter* _playEpub;
    VoiceDocument _vcurrent;
};

#endif // __EPUB__VIEW__H