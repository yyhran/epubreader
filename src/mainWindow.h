#ifndef __MAIN_WINDOW__H
#define __MAIN_WINDOW__H

#include <QMainWindow>
#include <QListWidget>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAction>
#include <QMenu>
// #include "epubView.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

private:
    auto initLayout() -> void;
    auto initSlots() -> void;
};

#endif // __MAIN_WINDOW__H