#include "baseformlayer.h"

#include <QInputDialog>

BaseFormLayer::BaseFormLayer(QWidget* parent, Layers* layers) : BaseForm(parent, false, true)
{
    _layers_original = layers;
    _layers_modified = layersCreateCopy(_layers_original);
}

BaseFormLayer::~BaseFormLayer()
{
    layersDelete(_layers_modified);
}

void BaseFormLayer::revertConfig()
{
    layersCopy(_layers_original, _layers_modified);
    layerSelectedEvent(currentLayer());
    
    BaseForm::revertConfig();
}

void BaseFormLayer::applyConfig()
{
    layersCopy(_layers_modified, _layers_original);
    
    BaseForm::applyConfig();
}

void BaseFormLayer::afterLayerAdded(void*)
{
}

void BaseFormLayer::configChangeEvent()
{
    layerApply(layersGetLayer(_layers_modified, currentLayer()));
    layersValidate(_layers_modified);
    
    BaseForm::configChangeEvent();
}

QStringList BaseFormLayer::getLayers()
{
    QStringList result;
    
    for (int i = 0; i < layersCount(_layers_modified); i++)
    {
        result << QString::fromUtf8(layersGetName(_layers_modified, i));
    }
    
    return result;
}

void BaseFormLayer::layerAddedEvent()
{
    QString layer_name = QInputDialog::getText(this, tr("Create layer"), tr("Layer name :"), QLineEdit::Normal, tr("Unnamed layer"));
    if (not layer_name.isEmpty())
    {
        int layer = layersAddLayer(_layers_modified, NULL);
        if (layer >= 0)
        {
            layersSetName(_layers_modified, layer, layer_name.toUtf8().data());
            
            BaseForm::layerAddedEvent();
            
            afterLayerAdded(layersGetLayer(_layers_modified, layer));
        }
    }
}

void BaseFormLayer::layerDeletedEvent(int layer)
{
    layersDeleteLayer(_layers_modified, layer);
    
    BaseForm::layerDeletedEvent(layer);
}

void BaseFormLayer::layerMovedEvent(int layer, int new_position)
{
    layersMove(_layers_modified, layer, new_position);
    
    BaseForm::layerMovedEvent(layer, new_position);
}

void BaseFormLayer::layerRenamedEvent(int layer, QString new_name)
{
    layersSetName(_layers_modified, layer, new_name.toUtf8().data());
    
    BaseForm::layerRenamedEvent(layer, new_name);
}

void BaseFormLayer::layerSelectedEvent(int layer)
{
    layerGetCopy(layersGetLayer(_layers_modified, layer));
    
    BaseForm::layerSelectedEvent(layer);
}
