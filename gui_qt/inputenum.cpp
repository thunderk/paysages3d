#include "inputenum.h"

#include <QLabel>
#include <QComboBox>

InputEnum::InputEnum(QWidget* form, QString label, int* value, const QStringList& values) : BaseInput(form, label)
{
    _value = value;
    
    _preview = new QWidget(form);
    _control = new QComboBox(form);
    ((QComboBox*)_control)->addItems(values);
    
    connect(_control, SIGNAL(currentIndexChanged(int)), this, SLOT(applyValue()));
}

void InputEnum::applyValue()
{
    *_value = ((QComboBox*)_control)->currentIndex();

    BaseInput::applyValue();
}

void InputEnum::revert()
{
    ((QComboBox*)_control)->setCurrentIndex(*_value);

    BaseInput::revert();
}
