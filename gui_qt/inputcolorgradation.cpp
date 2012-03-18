#include "inputcolorgradation.h"

#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QColorDialog>
#include "dialogcolorgradation.h"
#include "previewcolorgradation.h"
#include "tools.h"

InputColorGradation::InputColorGradation(QWidget* form, QString label, ColorGradation* value) : BaseInput(form, label)
{
    _value = value;
            
    _preview = new PreviewColorGradation(form, value, COLORGRADATIONBAND_FINAL);
    _preview->setMinimumSize(200, 20);

    _control = new QPushButton(tr("Edit"), form);
    _control->setMaximumWidth(150);

    connect((QPushButton*)_control, SIGNAL(clicked()), this, SLOT(editGradation()));
}

void InputColorGradation::updatePreview()
{
    _preview->update();
    BaseInput::updatePreview();
}

void InputColorGradation::applyValue()
{
    BaseInput::applyValue();
}

void InputColorGradation::revert()
{
    BaseInput::revert();
}

void InputColorGradation::editGradation()
{
    ColorGradation* gradation;
    
    gradation = colorGradationCreate();
    colorGradationCopy(_value, gradation);
    if (DialogColorGradation::getGradation(_preview, gradation))
    {
        colorGradationCopy(gradation, _value);
        applyValue();
    }
    colorGradationDelete(gradation);
}
