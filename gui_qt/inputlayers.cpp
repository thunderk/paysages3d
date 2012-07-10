#include "inputlayers.h"

#include <QLabel>
#include <QPushButton>

InputLayers::InputLayers(QWidget* form, QString label, Layers* value):
    BaseInput(form, label)
{
    _value = value;
    
    _preview = new QLabel(form);
    ((QLabel*)_preview)->setAlignment(Qt::AlignCenter);
    _control = new QPushButton(tr("Editer"), form);
}

void InputLayers::updatePreview()
{
    ((QLabel*)_preview)->setText(tr("%1 layers").arg(layersCount(_value)));

    BaseInput::updatePreview();
}
