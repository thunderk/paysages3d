#include "baseinput.h"

BaseInput::BaseInput(QWidget* form, QString label):
    QObject(form)
{
    _label = new QLabel(label);
    
    _label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _label->setWordWrap(true);
}

void BaseInput::updatePreview()
{
}

void BaseInput::applyValue()
{
    updatePreview();
    emit(valueChanged());
}

void BaseInput::revert()
{
    updatePreview();
}
