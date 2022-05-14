#ifndef __FONT_SLIDER__H
#define __FONT_SLIDER__H

#include <QLabel>
#include <QToolButton>
#include <QSlider>
#include <QHBoxLayout>

class FontSlider : public QWidget
{
    Q_OBJECT
public:
    FontSlider(int minValue = 0, int maxValue = 100, QWidget* parent = Q_NULLPTR);
    ~FontSlider();

    auto getValue() const noexcept -> int { return this->slider_->value(); }
    auto resizeValue() const noexcept -> void { this->slider_->setValue(100); }

signals:
    void valueChanged(int);    

private slots:
   void onAddBtnClicked();
   void onSubBtnClicked();
   void onValueChanged(int value);

private:
    QLabel* label_;
    QToolButton* addBtn_;
    QSlider* slider_;
    QToolButton* subBtn_;
};

#endif // __FONT_SLIDER__H