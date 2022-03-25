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
    auto data = e.readerMenu();
    qDebug() << "*data size: " << data.size();
    return a.exec();
}