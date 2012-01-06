#include "inputcolor.h"

#include <QLabel>
#include <QPushButton>

InputColor::InputColor(QWidget* form, QString label, Color* value):
    BaseInput(form, label),
    _value(value)
{
    _preview = new QWidget(form);
    _control = new QPushButton("Edit", form);
}

void InputColor::applyValue()
{
    BaseInput::applyValue();
}

void InputColor::revert()
{
}
