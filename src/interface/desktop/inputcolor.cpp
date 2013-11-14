#include "inputcolor.h"

#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QColorDialog>

#include "lighting/SmallPreviewColor.h"
#include "tools.h"

InputColor::InputColor(QWidget* form, QString label, Color* value):
    BaseInput(form, label),
    _original(value)
{
    _preview = new SmallPreviewColor(form, &_edited);
    _preview->setMinimumSize(50, 20);
    _control = new QPushButton(tr("Edit"), form);
    _control->setMaximumWidth(150);

    connect((QPushButton*)_control, SIGNAL(clicked()), this, SLOT(chooseColor()));
}

void InputColor::updatePreview()
{
    _preview->update();
    BaseInput::updatePreview();
}

void InputColor::applyValue()
{
    *_original = _edited;
    BaseInput::applyValue();
}

void InputColor::revert()
{
    _edited = *_original;
    BaseInput::revert();
}

void InputColor::chooseColor()
{
    QColor col = QColorDialog::getColor(colorToQColor(_edited), _control);
    if (col.isValid())
    {
        _edited.r = col.redF();
        _edited.g = col.greenF();
        _edited.b = col.blueF();
        applyValue();
    }
}
