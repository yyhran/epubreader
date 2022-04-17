#include "fontSlider.h"

FontSlider::FontSlider(int minValue, int maxValue, QWidget *parent)
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

FontSlider::~FontSlider() {}

void FontSlider::onAddBtnClicked()
{
    int value = this->_slider->value();
    value = qMin(this->_slider->maximum(), value + 10);
    this->_slider->setValue(value);
}

void FontSlider::onSubBtnClicked()
{
    int value = this->_slider->value();
    value = qMax(this->_slider->minimum(), value - 10);
    this->_slider->setValue(value);
}

void FontSlider::onValueChanged(int value)
{
    this->_label->setText(tr(QString("%1%").arg(value).toStdString().c_str()));
    emit this->valueChanged(value);
}
