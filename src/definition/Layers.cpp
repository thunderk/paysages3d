#include "Layers.h"

Layers::Layers(BaseDefinition* parent, LayerConstructor layer_constructor):
    BaseDefinition(parent), layer_constructor(layer_constructor)
{
    max_layer_count = 100;
    null_layer = layer_constructor(this);
}

Layers::~Layers()
{
    clear();
    delete null_layer;
}

void Layers::copy(BaseDefinition* destination_) const
{
    BaseDefinition::copy(destination_);

    Layers* destination = (Layers*)destination_;

    destination->clear();

    destination->max_layer_count = max_layer_count;

    null_layer->copy(destination->null_layer);

    for (auto layer: layers)
    {
        int position = destination->addLayer();
        BaseDefinition* new_layer = destination->getLayer(position);
        layer->copy(new_layer);
    }
}

Layers* Layers::newCopy() const
{
    Layers* result = new Layers(NULL, layer_constructor);
    copy(result);
    return result;
}

void Layers::setMaxLayerCount(int max_layer_count)
{
    this->max_layer_count = max_layer_count;
    // TODO Delete overlimit layers ?
}

int Layers::count() const
{
    return layers.size();
}

BaseDefinition* Layers::getLayer(int position) const
{
    if (position >= 0 and position < (int)layers.size())
    {
        return layers[position];
    }
    else
    {
        qWarning("Asked for a undefined layer %d on a total of %d", position, layers.size());
        return null_layer;
    }
}

int Layers::findLayer(BaseDefinition* layer) const
{
    int i = 0;
    for (auto it:layers)
    {
        if (it == layer)
        {
            return i;
        }
        i++;
    }
    qWarning("Layer %p not found, on a total of %d, returning %d", layer, layers.size(), -1);
    return -1;
}

int Layers::addLayer(BaseDefinition* layer)
{
    if ((int)layers.size() < max_layer_count)
    {
        layers.push_back(layer);
        addChild(layer);
        return layers.size() - 1;
    }
    else
    {
        qWarning("Add layer ignored because limit of %d reached", max_layer_count);
        return -1;
    }
}

int Layers::addLayer()
{
    return addLayer(layer_constructor(this));
}

void Layers::removeLayer(int position)
{
    if (position >= 0 and position < (int)layers.size())
    {
        BaseDefinition* removed = layers[position];
        removeChild(removed);
        layers.erase(layers.begin() + position);
        delete removed;
    }
}

void Layers::removeLayer(BaseDefinition* layer)
{
    removeLayer(findLayer(layer));
}

void Layers::moveLayer(int old_position, int new_position)
{
    if (old_position >= 0 and old_position < (int)layers.size() and new_position >= 0 and new_position < (int)layers.size())
    {
        BaseDefinition* layer = layers[old_position];
        layers.erase(layers.begin() + old_position);
        layers.insert(layers.begin() + new_position, layer);
    }
}

void Layers::moveLayer(BaseDefinition* layer, int new_position)
{
    moveLayer(findLayer(layer), new_position);
}

void Layers::clear()
{
    while (layers.size() > 0)
    {
        removeLayer(0);
    }
}
