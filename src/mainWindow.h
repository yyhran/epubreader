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
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

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

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent* event);

private:
    auto initLayout() -> void;
    auto setMenu() -> void;
    auto setHomeWidge() -> void;
    auto setViewWidget() -> void;
    auto setBottomWidget(QMainWindow* viewWidget) -> void;
    auto openFile(const QString& fileName) -> void;
    auto setToc() -> void;

private:
    QStackedWidget* stackedWidget_;
    QTreeWidget* treeWidget_;
    FontSlider* fontSlider_;
    EPUB::EpubView* _epubView;
    MetaInfo metaInfo_;
    TocMap tocMap_;
    QFont font_;
};

#endif // __MAIN_WINDOW__H