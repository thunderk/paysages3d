#include "inputcamera.h"

#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QColorDialog>

#include "dialogwanderer.h"

InputCamera::InputCamera(QWidget* form, QString label, CameraDefinition* value):BaseInput(form, label)
{
    _value = value;

    _preview = new QWidget(form);

    _control = new QPushButton("Edit", form);
    _control->setMaximumWidth(150);

    connect((QPushButton*)_control, SIGNAL(clicked()), this, SLOT(editCamera()));
}

void InputCamera::updatePreview()
{
    //_preview->update();

    BaseInput::updatePreview();
}

void InputCamera::applyValue()
{
    BaseInput::applyValue();
}

void InputCamera::revert()
{
    BaseInput::revert();
}

void InputCamera::editCamera()
{
    DialogWanderer* dialog = new DialogWanderer(_control, _value, true);
    dialog->exec();

    delete dialog;

    // TODO Only if camera was edited
    applyValue();
}
