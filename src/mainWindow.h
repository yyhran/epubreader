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
#include <QPushButton>
#include <QLabel>
#include <QToolButton>

#include "epubView.h"

class FontSlider : public QWidget
{
    Q_OBJECT
public:
    FontSlider(int minValue = 0, int maxValue = 100, QWidget* parent = Q_NULLPTR)
        : QWidget(parent)
        , _label(new QLabel(this))
        , _addBtn(new QToolButton(this))
        , _slider(new QSlider(this))
        , _subBtn(new QToolButton(this))
    {
        Q_ASSERT(maxValue >= 100);
        this->setMaximumWidth(260);
        this->_label->setText(tr("100%"));
        this->_label->setFixedWidth(40);
        this->_slider->setOrientation(Qt::Orientation::Horizontal);
        this->_slider->setMinimum(minValue);
        this->_slider->setMaximum(maxValue);
        this->_slider->setValue(100);
        this->_slider->setMaximumWidth(160);

        this->_addBtn->setText(tr("+"));
        this->_addBtn->setToolTip(tr("add font size"));
        this->_subBtn->setText(tr("-"));
        this->_subBtn->setToolTip(tr("sub font size"));

        auto layout = new QHBoxLayout(this);
        layout->setSpacing(4);
        layout->addWidget(_label);
        layout->addWidget(_subBtn);
        layout->addWidget(_slider);
        layout->addWidget(_addBtn);
        this->setLayout(layout);

        this->connect(this->_addBtn, SIGNAL(clicked()), this, SLOT(onAddBtnClicked()));
        this->connect(this->_subBtn, SIGNAL(clicked()), this, SLOT(onSubBtnClicked()));
        this->connect(this->_slider, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));
    }
    ~FontSlider() {}

    auto getValue() const noexcept -> int { return this->_slider->value(); }
    auto resizeValue() const noexcept -> void { this->_slider->setValue(100); }

signals:
    void valueChanged(int);    

private slots:
   void onAddBtnClicked()
   {
       int value = this->_slider->value();
       value = qMin(this->_slider->maximum(), value + 10);
       this->_slider->setValue(value);
   }

   void onSubBtnClicked()
   {
       int value = this->_slider->value();
       value = qMax(this->_slider->minimum(), value - 10);
       this->_slider->setValue(value);
   }

   void onValueChanged(int value)
   {
       this->_label->setText(tr(QString("%1%").arg(value).toStdString().c_str()));
       emit this->valueChanged(value);
   }

private:
    QLabel* _label;
    QToolButton* _addBtn;
    QSlider* _slider;
    QToolButton* _subBtn;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    using TocMap = QMap<QString, EPUB::EpubToc>;
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
    EPUB::EpubView* _epubView;
    MetaInfo _metaInfo;
    TocMap _tocMap;
    QFont _font;
};

#endif // __MAIN_WINDOW__H