#include "baseformlayer.h"

#include <QInputDialog>
#include "Layers.h"

BaseFormLayer::BaseFormLayer(QWidget* parent, Layers* layers) : BaseForm(parent, false, true)
{
    _layers_original = NULL;
    _layers_modified = NULL;
    if (layers)
    {
        _layers_original = layers;
        _layers_modified = _layers_original->newCopy();
    }
}

BaseFormLayer::~BaseFormLayer()
{
    if (_layers_modified)
    {
        delete _layers_modified;
    }
}

void BaseFormLayer::revertConfig()
{
    if (_layers_original && _layers_modified)
    {
        _layers_original->copy(_layers_modified);
        layerSelectedEvent(currentLayer());
    }

    BaseForm::revertConfig();
}

void BaseFormLayer::applyConfig()
{
    if (_layers_original && _layers_modified)
    {
        _layers_modified->copy(_layers_original);
    }

    BaseForm::applyConfig();
}

void BaseFormLayer::setLayers(Layers* layers)
{
    _layers_original = layers;
    if (_layers_modified)
    {
        delete _layers_modified;
    }
    _layers_modified = _layers_original->newCopy();
    revertConfig();
}

void BaseFormLayer::afterLayerAdded(void*)
{
}

void BaseFormLayer::configChangeEvent()
{
    if (_layers_modified)
    {
        layerWriteCurrentTo(_layers_modified->getLayer(currentLayer()));
        _layers_modified->validate();
    }

    BaseForm::configChangeEvent();
}

QStringList BaseFormLayer::getLayers()
{
    QStringList result;

    if (_layers_modified)
    {
        for (int i = 0; i < _layers_modified->count(); i++)
        {
            result << QString::fromStdString(_layers_modified->getLayer(i)->getName());
        }
    }

    return result;
}

void BaseFormLayer::layerAddedEvent()
{
    if (_layers_modified)
    {
        QString layer_name = QInputDialog::getText(this, tr("Create layer"), tr("Layer name :"), QLineEdit::Normal, tr("Unnamed layer"));
        if (not layer_name.isEmpty())
        {
            int layer = _layers_modified->addLayer();
            if (layer >= 0)
            {
                _layers_modified->getLayer(layer)->setName(layer_name.toStdString());

                BaseForm::layerAddedEvent();

                afterLayerAdded(_layers_modified->getLayer(layer));
            }
        }
    }
}

void BaseFormLayer::layerDeletedEvent(int layer)
{
    if (_layers_modified)
    {
        _layers_modified->removeLayer(layer);
    }

    BaseForm::layerDeletedEvent(layer);
}

void BaseFormLayer::layerMovedEvent(int layer, int new_position)
{
    if (_layers_modified)
    {
        _layers_modified->moveLayer(layer, new_position);
    }

    BaseForm::layerMovedEvent(layer, new_position);
}

void BaseFormLayer::layerRenamedEvent(int layer, QString new_name)
{
    if (_layers_modified)
    {
        _layers_modified->getLayer(layer)->setName(new_name.toStdString());
    }

    BaseForm::layerRenamedEvent(layer, new_name);
}

void BaseFormLayer::layerSelectedEvent(int layer)
{
    if (_layers_modified && layer >= 0)
    {
        layerReadCurrentFrom(_layers_modified->getLayer(layer));
    }

    BaseForm::layerSelectedEvent(layer);
}
