#include "baseinput.h"

BaseInput::BaseInput(QWidget* form, QString label):
    QObject(form)
{
    _label = new QLabel(label);
    
    _label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _label->setWordWrap(true);
    
    _visibility_value = NULL;
    _enabled_value = NULL;
    
    _visible = true;
    _enabled = true;
}

void BaseInput::setVisibilityCondition(int* value, int condition)
{
    _visibility_value = value;
    _visibility_condition = condition;
}

void BaseInput::setEnabledCondition(int* value, int condition)
{
    _enabled_value = value;
    _enabled_condition = condition;
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

void BaseInput::checkVisibility(bool enabled)
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
    
    if (enabled && (!_enabled_value || *_enabled_value == _enabled_condition))
    {
        if (not _enabled)
        {
            _enabled = true;
            
            _preview->setEnabled(true);
            _control->setEnabled(true);
        }
    }
    else
    {
        if (_enabled)
        {
            _enabled = false;
            
            _preview->setEnabled(false);
            _control->setEnabled(false);
        }
    }
}
