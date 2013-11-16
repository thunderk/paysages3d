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
    Layers* destination = (Layers*)destination_;

    // don't call overridden method, it will copy again the children
    // FIXME ... but the definition name (and other future attributes) is not copied

    destination->clear();

    destination->max_layer_count = max_layer_count;

    null_layer->copy(destination->null_layer);

    QListIterator<BaseDefinition*> it(layers);
    while (it.hasNext())
    {
        int position = destination->addLayer();
        BaseDefinition* new_layer = destination->getLayer(position);
        it.next()->copy(new_layer);
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
    return layers.count();
}

BaseDefinition* Layers::getLayer(int position) const
{
    if (position >= 0 and position < layers.size())
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
    int result = layers.indexOf(layer);
    if (result < 0)
    {
        qWarning("Layer %p not found, on a total of %d, returning %d", layer, layers.size(), result);
    }
    return result;
}

int Layers::addLayer(BaseDefinition* layer)
{
    if (layers.size() < max_layer_count)
    {
        layers.append(layer);
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
    if (position >= 0 and position < layers.size())
    {
        BaseDefinition* removed = layers.takeAt(position);
        removeChild(removed);
        delete removed;
    }
}

void Layers::removeLayer(BaseDefinition* layer)
{
    removeLayer(findLayer(layer));
}

void Layers::moveLayer(int old_position, int new_position)
{
    if (old_position >= 0 and old_position < layers.size() and new_position >= 0 and new_position < layers.size())
    {
        layers.move(old_position, new_position);
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
