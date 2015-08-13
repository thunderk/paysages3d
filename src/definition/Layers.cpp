#include "Layers.h"

#include "PackStream.h"
#include "Logs.h"

Layers::Layers(DefinitionNode* parent, const std::string &name, LayerConstructor layer_constructor):
    DefinitionNode(parent, name), layer_constructor(layer_constructor)
{
    max_layer_count = 100;
    null_layer = layer_constructor(this);
}

Layers::~Layers()
{
    clear();
    delete null_layer;
}

void Layers::save(PackStream *stream) const
{
    int layer_count = (int)layers.size();
    stream->write(&layer_count);

    for (int i = 0; i < layer_count; i++)
    {
        stream->write(layers[i]->getName());
        layers[i]->save(stream);
    }
}

void Layers::load(PackStream *stream)
{
    int layer_count;
    stream->read(&layer_count);

    if (layer_count > max_layer_count)
    {
        layer_count = max_layer_count;
    }
    clear();
    for (int i = 0; i < layer_count; i++)
    {
        int position = addLayer();
        if (position >= 0)
        {
            layers[position]->setName(stream->readString());
            layers[position]->load(stream);
        }
    }
}

void Layers::copy(DefinitionNode* destination_) const
{
    DefinitionNode::copy(destination_);

    Layers* destination = (Layers*)destination_;

    destination->clear();

    destination->max_layer_count = max_layer_count;

    null_layer->copy(destination->null_layer);

    for (auto layer: layers)
    {
        int position = destination->addLayer();
        DefinitionNode* new_layer = destination->getLayer(position);
        layer->copy(new_layer);
    }
}

Layers* Layers::newCopy() const
{
    Layers* result = new Layers(NULL, getName(), layer_constructor);
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

DefinitionNode* Layers::getLayer(int position) const
{
    if (position >= 0 and position < (int)layers.size())
    {
        return layers[position];
    }
    else
    {
        Logs::warning() << "Asked for a undefined layer " << position << " on a total of " << (int)layers.size() << std::endl;
        return null_layer;
    }
}

int Layers::findLayer(DefinitionNode* layer) const
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
    Logs::warning() << "Layer " << layer << " (" << layer->getName() << " not found, on a total of " << (int)layers.size() << std::endl;
    return -1;
}

int Layers::addLayer(DefinitionNode* layer)
{
    if ((int)layers.size() < max_layer_count)
    {
        layers.push_back(layer);
        addChild(layer);
        return layers.size() - 1;
    }
    else
    {
        Logs::warning() << "Add layer ignored because limit of " << max_layer_count << " reached" << std::endl;
        delete layer;
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
        DefinitionNode* removed = layers[position];
        removeChild(removed);
        layers.erase(layers.begin() + position);
        delete removed;
    }
    else
    {
        Logs::warning() << "Removing unknown layer " << position << " on " << (int)layers.size() << " from '" << getName() << "'" << std::endl;
    }
}

void Layers::removeLayer(DefinitionNode* layer)
{
    removeLayer(findLayer(layer));
}

void Layers::moveLayer(int old_position, int new_position)
{
    if (old_position >= 0 and old_position < (int)layers.size() and new_position >= 0 and new_position < (int)layers.size())
    {
        DefinitionNode* layer = layers[old_position];
        layers.erase(layers.begin() + old_position);
        layers.insert(layers.begin() + new_position, layer);
    }
}

void Layers::moveLayer(DefinitionNode* layer, int new_position)
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

DefinitionNode *Layers::findChildByName(const std::string name)
{
    DefinitionNode *result = DefinitionNode::findChildByName(name);
    if (result)
    {
        return result;
    }
    else
    {
        int position = addLayer();
        if (position >= 0)
        {
            result = getLayer(position);
            result->setName(name);
            return result;
        }
        else
        {
            return NULL;
        }
    }
}
