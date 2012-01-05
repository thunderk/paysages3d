#include "baseinput.h"

#include <QLabel>

BaseInput::BaseInput(QWidget* form, QString label):
    QObject(form)
{
    _label = new QLabel(label);
}

void BaseInput::applyValue()
{
    emit(valueChanged());
}
