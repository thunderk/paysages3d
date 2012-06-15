#include "inputdouble.h"

#include <QLabel>
#include "math.h"

InputDouble::InputDouble(QWidget* form, QString label, float* value, float min, float max, float small_step, float large_step):
    BaseInput(form, label),
    value(value), min(min), max(max), small_step(small_step), large_step(large_step)
{
    slider = new QSlider(form);

    slider->setOrientation(Qt::Horizontal);
    slider->setMinimumWidth(200);
    slider->setMaximumWidth(400);

    slider->setMinimum(0);
    slider->setMaximum(round((max - min) / small_step));
    slider->setValue(round((*value - min) / small_step));

    slider->setTickInterval(round(large_step / small_step));
    slider->setTickPosition(QSlider::TicksBelow);

    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(applyValue()));

    _preview = new QLabel(form);
    ((QLabel*)_preview)->setAlignment(Qt::AlignCenter);
    _control = slider;
}

void InputDouble::updatePreview()
{
    ((QLabel*)_preview)->setText(QString::number(*value, 'g', 3));

    BaseInput::updatePreview();
}

void InputDouble::applyValue()
{
    int ivalue = slider->value();
    if (ivalue == slider->maximum())
    {
        *value = max;
    }
    else
    {
        *value = min + ((float)ivalue) * small_step;
    }
    if (fabs(*value) < 0.0000001)
    {
        *value = 0.0;
    }

    BaseInput::applyValue();
}

void InputDouble::revert()
{
    slider->setValue(round((*value - min) / small_step));

    BaseInput::revert();
}
