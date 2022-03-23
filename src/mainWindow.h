#ifndef __MAIN_WINDOW__H
#define __MAIN_WINDOW__H

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();
};

#endif // __MAIN_WINDOW__H