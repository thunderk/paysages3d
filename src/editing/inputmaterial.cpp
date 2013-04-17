#include "inputmaterial.h"

#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QColorDialog>
#include "dialogmaterial.h"
#include "previewmaterial.h"

InputMaterial::InputMaterial(QWidget* form, QString label, SurfaceMaterial* value) : BaseInput(form, label)
{
    _value = value;
    
    _preview = new SmallMaterialPreview(form, value);
    _preview->setMinimumSize(100, 40);
    
    _control = new QPushButton(tr("Edit"), form);
    _control->setMaximumWidth(150);

    connect((QPushButton*)_control, SIGNAL(clicked()), this, SLOT(editMaterial()));
}

void InputMaterial::updatePreview()
{
    _preview->update();

    BaseInput::updatePreview();
}

void InputMaterial::applyValue()
{
    BaseInput::applyValue();
}

void InputMaterial::revert()
{
    BaseInput::revert();
}

void InputMaterial::editMaterial()
{
    if (DialogMaterial::getMaterial(_control, _value))
    {
        applyValue();
    }
}
