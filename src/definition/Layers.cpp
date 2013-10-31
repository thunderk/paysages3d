#include "Layers.h"

Layers::Layers(BaseDefinition* parent, LayerConstructor layer_constructor, LayerType* legacy_type):
    BaseDefinition(parent), layer_constructor(layer_constructor)
{
    if (legacy_type)
    {
        this->legacy_type = *legacy_type;
    }
    max_layer_count = 100;
    null_layer = layer_constructor(this);
}

Layers::~Layers()
{
    clear();
    delete null_layer;
}

void Layers::copy(BaseDefinition* destination_)
{
    Layers* destination = (Layers*)destination_;

    // don't call overridden method, it will copy again the children
    // FIXME ... but the definition name (and other future attributes) is not copied

    destination->clear();

    destination->max_layer_count = max_layer_count;
    destination->legacy_type = legacy_type;

    null_layer->copy(destination->null_layer);

    QListIterator<BaseDefinition*> it(layers);
    while (it.hasNext())
    {
        int position = destination->addLayer();
        BaseDefinition* new_layer = destination->getLayer(position);
        it.next()->copy(new_layer);
    }
}

void Layers::setMaxLayerCount(int max_layer_count)
{
    this->max_layer_count = max_layer_count;
    // TODO Delete overlimit layers ?
}

int Layers::count()
{
    return layers.count();
}

BaseDefinition* Layers::getLayer(int position)
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

int Layers::findLayer(BaseDefinition* layer)
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

// Transitional C-API

BaseDefinition* _legacyLayerConstructor(Layers* layers)
{
    return new LegacyLayer(layers, &layers->legacy_type);
}

Layers* layersCreate(LayerType type, int max_layer_count)
{
    Layers* result = new Layers(NULL, _legacyLayerConstructor, &type);
    result->setMaxLayerCount(max_layer_count);
    return result;
}

Layers* layersCreateCopy(Layers* original)
{
    Layers* result = new Layers(NULL, _legacyLayerConstructor, &original->legacy_type);
    original->copy(result);
    return result;
}

void layersDelete(Layers* layers)
{
    delete layers;
}

void layersCopy(Layers* source, Layers* destination)
{
    source->copy(destination);
}

void layersValidate(Layers* layers)
{
    layers->validate();
}

void layersSave(PackStream* stream, Layers* layers)
{
    layers->save(stream);
}

void layersLoad(PackStream* stream, Layers* layers)
{
    layers->load(stream);
}

const char* layersGetName(Layers* layers, int layer)
{
    return ((LegacyLayer*)(layers->getLayer(layer)))->getLegacyName();
}

void layersSetName(Layers* layers, int layer, const char* name)
{
    layers->getLayer(layer)->setName(name);
}

void layersClear(Layers* layers)
{
    layers->clear();
}

int layersCount(Layers* layers)
{
    return layers->count();
}

void* layersGetLayer(Layers* layers, int layer)
{
    LegacyLayer* legacy = (LegacyLayer*)(layers->getLayer(layer));
    return legacy->getLegacyDefinition();
}

int layersAddLayer(Layers* layers, void* definition)
{
    int position;
    LegacyLayer* legacy = new LegacyLayer(layers, &layers->legacy_type);
    if (definition)
    {
        layers->legacy_type.callback_copy(definition, legacy->getLegacyDefinition());
    }
    position = layers->addLayer(legacy);
    return position;
}

void layersDeleteLayer(Layers* layers, int layer)
{
    layers->removeLayer(layer);
}

void layersMove(Layers* layers, int layer, int new_position)
{
    layers->moveLayer(layer, new_position);
}
