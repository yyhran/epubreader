#ifndef __MAIN_WINDOW__H
#define __MAIN_WINDOW__H

#include <QMainWindow>
#include <QListWidget>
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

private slots:
    void openFile();
    void gotoPage(int page = 0);

    void keyPressEvent(QKeyEvent* e) override { this->_epubView->keyPressEvent(e); }

private:
    auto initLayout() -> void;
    auto setMenu() -> void;
    auto setHomeWidge() -> void;
    auto setViewWidget() -> void;

private:
    QStackedWidget* _stackedWidget;
    QListWidget* _listWidget;
    EpubView* _epubView;
};

#endif // __MAIN_WINDOW__H