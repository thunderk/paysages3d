#include "inputdouble.h"

#include <QLabel>
#include "math.h"

InputDouble::InputDouble(QWidget* form, QString label, double* value, double min, double max, double small_step, double large_step):
    BaseInput(form, label)
{
    _value = value;
    _min = min;
    _max = max;
    _small_step = small_step;
    _large_step = large_step;
    
    _slider = new QSlider(form);

    _slider->setOrientation(Qt::Horizontal);
    _slider->setMinimumWidth(200);
    _slider->setMaximumWidth(400);

    _slider->setMinimum(0);
    _slider->setMaximum(round((max - min) / small_step));
    _slider->setValue(round((*value - min) / small_step));

    _slider->setTickInterval(round(large_step / small_step));
    _slider->setTickPosition(QSlider::TicksBelow);

    connect(_slider, SIGNAL(valueChanged(int)), this, SLOT(applyValue()));

    _preview = new QLabel(form);
    ((QLabel*)_preview)->setAlignment(Qt::AlignCenter);
    _control = _slider;
}

void InputDouble::updatePreview()
{
    ((QLabel*)_preview)->setText(QString::number(*_value, 'g', 3));

    BaseInput::updatePreview();
}

double InputDouble::getValue()
{
    double result;
    int ivalue = _slider->value();
    if (ivalue == _slider->maximum())
    {
        result = _max;
    }
    else
    {
        result = _min + ((double)ivalue) * _small_step;
    }
    if (fabs(*_value) < 0.0000001)
    {
        result = 0.0;
    }
    return result;
}

void InputDouble::applyValue()
{
    *_value = getValue();
    BaseInput::applyValue();
}

void InputDouble::revert()
{
    double value = round((*_value - _min) / _small_step);
    if (value != _slider->value())
    {
        _slider->setValue(value);
    }

    BaseInput::revert();
}
