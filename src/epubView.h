#ifndef __EPUB_VIEW__H
#define __EPUB_VIEW__H

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
#include <QHeaderView>
#include <QPainter>

#include "edocument.h"

namespace EPUB {

class EpubView : public QWidget 
{
    Q_OBJECT

public:
    explicit EpubView(QWidget* parent = 0);
    ~EpubView();

    auto loadFile(const QString& path) -> void;
    auto getToc() -> QList<EpubToc> { return this->_document->getToc(); }
    auto setF(const QString& fileName) -> void { this->_document->setF(fileName); }

    void paintEvent(QPaintEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;

private:
    auto scroll(int amount) -> void;
    auto scrollPage(int amount) -> void;

private:
    Document* _document;
    int _offset;
};

}

#endif // __EPUB__VIEW__H
