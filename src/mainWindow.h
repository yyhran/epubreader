#ifndef __MAIN_WINDOW__H
#define __MAIN_WINDOW__H

#include <QMainWindow>
#include <QTreeWidget>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QFileDialog>
#include <QDebug>
#include <QLabel>
#include <QDockWidget>
#include "epubView.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

    void keyPressEvent(QKeyEvent* e) override { this->_epubView->keyPressEvent(e); } // just forward

private slots:
    void openFile();
    void gotoStackedWidgetPage(int page = 0);

private:
    auto initLayout() -> void;
    auto setMenu() -> void;
    auto setHomeWidge() -> void;
    auto setViewWidget() -> void;
    auto setToc() -> void;

private:
    QStackedWidget* _stackedWidget;
    QTreeWidget* _treeWidget;
    EPUB::EpubView* _epubView;
};

#endif // __MAIN_WINDOW__H