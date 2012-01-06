#include "inputint.h"

#include <QLabel>

InputInt::InputInt(QWidget* form, QString label, int* value, int min, int max, int small_step, int large_step):
    BaseInput(form, label),
    value(value), min(min), max(max), small_step(small_step), large_step(large_step)
{
    slider = new QSlider(form);

    slider->setOrientation(Qt::Horizontal);
    slider->setMinimumWidth(150);
    slider->setMaximumWidth(400);

    slider->setMinimum(min);
    slider->setMaximum(max);
    slider->setValue(*value);

    slider->setTickInterval(large_step);
    slider->setTickPosition(QSlider::TicksBelow);

    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(applyValue()));

    _preview = new QLabel(form);
    _control = slider;
}

void InputInt::applyValue()
{
    *value = (int)slider->value();
    ((QLabel*)_preview)->setText(QString("%1").arg(*value));

    BaseInput::applyValue();
}

void InputInt::revert()
{
    slider->setValue(*value);
}
