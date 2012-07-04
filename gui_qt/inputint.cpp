#include "inputint.h"

#include <QLabel>

InputInt::InputInt(QWidget* form, QString label, int* value, int min, int max, int small_step, int large_step):
    BaseInput(form, label)
{
    _value = value;
    _min = min;
    _max = max;
    _small_step = small_step;
    _large_step = large_step;
    
    _slider = new QSlider(form);

    _slider->setOrientation(Qt::Horizontal);
    _slider->setMinimumWidth(150);
    _slider->setMaximumWidth(400);

    _slider->setMinimum(min);
    _slider->setMaximum(max);
    _slider->setValue(*value);

    _slider->setTickInterval(large_step);
    _slider->setTickPosition(QSlider::TicksBelow);

    connect(_slider, SIGNAL(valueChanged(int)), this, SLOT(applyValue()));

    _preview = new QLabel(form);
    ((QLabel*)_preview)->setAlignment(Qt::AlignCenter);
    _control = _slider;
}

void InputInt::updatePreview()
{
    ((QLabel*)_preview)->setText(QString("%1").arg(*_value));

    BaseInput::updatePreview();
}

void InputInt::applyValue()
{
    *_value = (int)_slider->value();

    BaseInput::applyValue();
}

void InputInt::revert()
{
    if (*_value != _slider->value())
    {
        _slider->setValue(*_value);
    }

    BaseInput::revert();
}
