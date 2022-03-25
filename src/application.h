#ifndef __APPLICATION__H
#define __APPLICATION__H

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


#endif // __APPLICATION__H