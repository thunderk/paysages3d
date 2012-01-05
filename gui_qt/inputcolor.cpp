#include "inputcolor.h"

#include <QLabel>

InputColor::InputColor(QWidget* form, QString label, Color color):
    BaseInput(form, label),
    _color(color)
{
    setObjectName("_inputcolor_");

    _preview = new QLabel(form);
    _control = new QLabel(form);
}

void InputColor::applyValue()
{
    emit(valueChanged());
}

void InputColor::revert()
{
}
