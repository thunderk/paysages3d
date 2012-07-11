#include "baseformlayer.h"

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
    if (layersAddLayer(_layers_modified, NULL) >= 0)
    {
        BaseForm::layerAddedEvent();
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
