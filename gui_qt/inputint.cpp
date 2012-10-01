#include "inputint.h"

#include <QLabel>

InputInt::InputInt(QWidget* form, QString label, int* value, int min, int max, int small_step, int large_step):
    InputDouble(form, label, &_dvalue, (double)min, (double)max, (double)small_step, (double)large_step)
{
    _value = value;
    _dvalue = (double)(*value);
}

void InputInt::updatePreview()
{
    ((QLabel*)_preview)->setText(QString("%1").arg(_dvalue, 0, 'f', 0));
}

void InputInt::applyValue()
{
    *_value = (int)_dvalue;

    InputDouble::applyValue();
}

void InputInt::revert()
{
    _dvalue = (double)(*_value);

    InputDouble::revert();
}
