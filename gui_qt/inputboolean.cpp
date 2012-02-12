#include "inputboolean.h"

#include <QLabel>
#include "math.h"

InputBoolean::InputBoolean(QWidget* form, QString label, int* value) : BaseInput(form, label)
{
    this->value = value;
    
    checkbox = new QCheckBox(form);

    connect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(applyValue()));

    _preview = new QLabel(form);
    _control = checkbox;
}

void InputBoolean::updatePreview()
{
    if (checkbox->checkState() == Qt::Checked)
    {
        ((QLabel*)_preview)->setText("Yes");
    }
    else
    {
        ((QLabel*)_preview)->setText("No");
    }

    BaseInput::updatePreview();
}

void InputBoolean::applyValue()
{
    if (checkbox->checkState() == Qt::Checked)
    {
        *value = 1;
    }
    else
    {
        *value = 0;
    }
    
    BaseInput::applyValue();
}

void InputBoolean::revert()
{
    if (*value)
    {
        checkbox->setCheckState(Qt::Checked);
    }
    else
    {
        checkbox->setCheckState(Qt::Unchecked);
    }

    BaseInput::revert();
}
