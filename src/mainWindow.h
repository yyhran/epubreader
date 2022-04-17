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
#include <QDockWidget>
#include <QStatusBar>
#include <QSlider>

#include "epubView.h"
#include "fontSlider.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    using TocMap = QMap<QTreeWidgetItem*, EPUB::EpubToc>;
    using MetaInfo = EPUB::Document::MetaInfo;

    MainWindow(QWidget* parent = 0);
    ~MainWindow();

private slots:
    void openFile();
    void gotoStackedWidgetPage(int page = 0);
    void gotoFile(QTreeWidgetItem* item, int index);

private:
    auto initLayout() -> void;
    auto setMenu() -> void;
    auto setHomeWidge() -> void;
    auto setViewWidget() -> void;
    auto setBottomWidget(QMainWindow* viewWidget) -> void;

    auto setToc() -> void;

private:
    QStackedWidget* _stackedWidget;
    QTreeWidget* _treeWidget;
    FontSlider* _fontSlider;
    EPUB::EpubView* _epubView;
    MetaInfo _metaInfo;
    TocMap _tocMap;
    QFont _font;
};

#endif // __MAIN_WINDOW__H