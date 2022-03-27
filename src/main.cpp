#include <iostream>
#include <QApplication>
#include "mainWindow.h"
#include "epubView.h"

int main(int argc, char** argv)
{
    QApplication a(argc, argv);
    // MainWindow w;
    // w.show();

    EpubView e;
    e.show();
    return a.exec();
}