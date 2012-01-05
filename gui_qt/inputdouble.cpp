#include "inputdouble.h"

#include <QLabel>

InputDouble::InputDouble(QWidget* form, QString label, double* value, double min, double max, double small_step, double large_step):
    BaseInput(form, label),
    value(value), min(min), max(max), small_step(small_step), large_step(large_step)
{
    setObjectName("_form_doubleslider_");

    slider = new QSlider(form);

    slider->setOrientation(Qt::Horizontal);
    slider->setMinimumWidth(150);
    slider->setMaximumWidth(400);

    slider->setMinimum(min / small_step);
    slider->setMaximum(max / small_step);
    slider->setValue(*value / small_step);

    slider->setTickInterval(large_step / small_step);
    slider->setTickPosition(QSlider::TicksBelow);

    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(applyValue()));

    _preview = new QLabel(form);
    _control = slider;
}

void InputDouble::applyValue()
{
    *value = ((double)slider->value()) * small_step;
    ((QLabel*)_preview)->setText(QString("%1").arg(*value));

    BaseInput::applyValue();
}

void InputDouble::revert()
{
    slider->setValue(*value / small_step);
}
