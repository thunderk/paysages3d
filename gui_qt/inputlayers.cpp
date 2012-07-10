#include "inputlayers.h"

#include <QLabel>
#include <QPushButton>
#include "dialoglayers.h"

InputLayers::InputLayers(QWidget* form, QString label, Layers* value, FormLayerBuilder form_builder):
    BaseInput(form, label)
{
    _value = value;
    _form_builder = form_builder;
    
    _preview = new QLabel(form);
    ((QLabel*)_preview)->setAlignment(Qt::AlignCenter);
    _control = new QPushButton(tr("Editer"), form);
    
    connect(_control, SIGNAL(clicked()), this, SLOT(editLayers()));
}

void InputLayers::updatePreview()
{
    ((QLabel*)_preview)->setText(tr("%1 layers").arg(layersCount(_value)));

    BaseInput::updatePreview();
}

void InputLayers::editLayers()
{
    if (DialogLayers::editLayers(_control, _value, _label->text(), _form_builder))
    {
    }
}
