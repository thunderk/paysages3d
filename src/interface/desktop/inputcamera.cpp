#include "inputcamera.h"

#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QColorDialog>

#include "dialogexplorer.h"

InputCamera::InputCamera(QWidget* form, QString label, CameraDefinition* value):BaseInput(form, label)
{
    _value = value;

    _preview = new QWidget(form);

    _control = new QPushButton(tr("Edit"), form);
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
    int result;
    
    DialogExplorer* dialog = new DialogExplorer(_control, _value, true);
    result = dialog->exec();

    delete dialog;

    if (result == QDialog::Accepted)
    {
        applyValue();
    }
}
