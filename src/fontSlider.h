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

    auto getValue() const noexcept -> int { return this->_slider->value(); }
    auto resizeValue() const noexcept -> void { this->_slider->setValue(100); }

signals:
    void valueChanged(int);    

private slots:
   void onAddBtnClicked();
   void onSubBtnClicked();
   void onValueChanged(int value);

private:
    QLabel* _label;
    QToolButton* _addBtn;
    QSlider* _slider;
    QToolButton* _subBtn;
};

#endif // __FONT_SLIDER__H