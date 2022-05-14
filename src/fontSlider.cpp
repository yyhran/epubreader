#include "fontSlider.h"

FontSlider::FontSlider(int minValue, int maxValue, QWidget *parent)
    : QWidget(parent)
    , label_(new QLabel(this))
    , addBtn_(new QToolButton(this))
    , slider_(new QSlider(this))
    , subBtn_(new QToolButton(this))
{
    Q_ASSERT(maxValue >= 100);
    
    this->setMaximumWidth(260);
    this->label_->setText(tr("100%"));
    this->label_->setFixedWidth(40);
    this->slider_->setOrientation(Qt::Orientation::Horizontal);
    this->slider_->setMinimum(minValue);
    this->slider_->setMaximum(maxValue);
    this->slider_->setValue(100);
    this->slider_->setMaximumWidth(160);

    this->addBtn_->setText(tr("+"));
    this->addBtn_->setToolTip(tr("add font size"));
    this->subBtn_->setText(tr("-"));
    this->subBtn_->setToolTip(tr("sub font size"));

    auto layout = new QHBoxLayout(this);
    layout->setSpacing(4);
    layout->addWidget(label_);
    layout->addWidget(subBtn_);
    layout->addWidget(slider_);
    layout->addWidget(addBtn_);
    this->setLayout(layout);

    this->connect(this->addBtn_, SIGNAL(clicked()), this, SLOT(onAddBtnClicked()));
    this->connect(this->subBtn_, SIGNAL(clicked()), this, SLOT(onSubBtnClicked()));
    this->connect(this->slider_, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));
}

FontSlider::~FontSlider() {}

void FontSlider::onAddBtnClicked()
{
    int value = this->slider_->value();
    value = qMin(this->slider_->maximum(), value + 10);
    this->slider_->setValue(value);
}

void FontSlider::onSubBtnClicked()
{
    int value = this->slider_->value();
    value = qMax(this->slider_->minimum(), value - 10);
    this->slider_->setValue(value);
}

void FontSlider::onValueChanged(int value)
{
    this->label_->setText(tr(QString("%1%").arg(value).toStdString().c_str()));
    emit this->valueChanged(value);
}
