#include "baseinput.h"

BaseInput::BaseInput(QWidget* form, QString label):
    QObject(form)
{
    _label = new QLabel(label);
    
    _label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _label->setWordWrap(true);
    
    _visibility_value = NULL;
    _visible = true;
}

void BaseInput::setVisibilityCondition(int* value, int condition)
{
    _visibility_value = value;
    _visibility_condition = condition;
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

void BaseInput::checkVisibility()
{
    if (!_visibility_value || *_visibility_value == _visibility_condition)
    {
        if (not _visible)
        {
            _visible = true;
            
            _label->show();
            _preview->show();
            _control->show();
        }
    }
    else
    {
        if (_visible)
        {
            _visible = false;
            
            _label->hide();
            _preview->hide();
            _control->hide();
        }
    }
}
