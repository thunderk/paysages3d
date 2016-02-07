#include "Layers.h"

#include "PackStream.h"
#include "Logs.h"
#include "LayersDiff.h"

Layers::Layers(DefinitionNode *parent, const string &name, LayerConstructor layer_constructor)
    : DefinitionNode(parent, name, "layers" + name), layer_constructor(layer_constructor) {
    max_layer_count = 100;
    null_layer = layer_constructor(this, "#NULL#");
}

Layers::~Layers() {
    delete null_layer;
}

void Layers::save(PackStream *stream) const {
    int layer_count = static_cast<int>(layers.size());
    stream->write(&layer_count);

    for (auto &layer : layers) {
        stream->write(layer->getName());
        layer->save(stream);
    }
}

void Layers::load(PackStream *stream) {
    int layer_count;
    stream->read(&layer_count);

    clear();
    for (int i = 0; i < layer_count; i++) {
        DefinitionNode *layer = layer_constructor(this, stream->readString());
        layer->load(stream);
        addLayer(*layer);
        delete layer;
    }
}

void Layers::copy(DefinitionNode *destination_) const {
    DefinitionNode::copy(destination_);

    Layers *destination = (Layers *)destination_;

    destination->clear();

    destination->max_layer_count = max_layer_count;

    null_layer->copy(destination->null_layer);

    for (auto layer : layers) {
        destination->addLayer(*layer);
    }
}

void Layers::setMaxLayerCount(int max_layer_count) {
    this->max_layer_count = max_layer_count;
    for (int i = getLayerCount(); i > max_layer_count; i--) {
        removeLayer(i - 1);
    }
}

int Layers::getLayerCount() const {
    return layers.size();
}

DefinitionNode *Layers::getLayer(int position) const {
    if (position >= 0 and position < (int)layers.size()) {
        return layers[position];
    } else {
        Logs::warning("Definition") << "Asked for a undefined layer " << position << " on a total of "
                                    << (int)layers.size() << endl;
        return null_layer;
    }
}

bool Layers::applyDiff(const DefinitionDiff *diff, bool backward) {
    auto layer_diff = (LayersDiff *)diff;
    LayersDiff::LayersDiffOp op = layer_diff->getOp();
    int layer_count = getLayerCount();

    if ((not backward and op == LayersDiff::LAYER_ADDED) or (backward and op == LayersDiff::LAYER_REMOVED)) {
        if (layer_count >= max_layer_count) {
            Logs::warning("Definition") << "Add layer ignored because limit of " << max_layer_count << " reached"
                                        << endl;
            return false;
        } else {
            int position = layer_diff->getLayer1();
            if (position < 0 or position > layer_count) {
                Logs::error("Definition") << "Add layer ignored because requested position was incorrect" << endl;
                return false;
            } else {
                DefinitionNode *layer = layer_constructor(NULL, "temp");
                layer_diff->restoreSavedLayer(layer);
                if (position == layer_count) {
                    layers.push_back(layer);
                } else {
                    layers.insert(layers.begin() + position, layer);
                }
                addChild(layer);
                return true;
            }
        }
    } else if ((not backward and op == LayersDiff::LAYER_REMOVED) or (backward and op == LayersDiff::LAYER_ADDED)) {
        int position = layer_diff->getLayer1();
        if (position < 0 or position >= layer_count) {
            Logs::warning("Definition") << "Removing unknown layer " << position << " on " << layer_count << " from '"
                                        << getName() << "'" << endl;
            return false;
        } else {
            DefinitionNode *removed = layers[position];
            removeChild(removed);
            layers.erase(layers.begin() + position);
            delete removed;
            return true;
        }
    }
    return false;
}

void Layers::generateInitDiffs(vector<const DefinitionDiff *> *diffs) const {
    int i = 0;
    for (auto layer : layers) {
        auto diff = new LayersDiff(this, LayersDiff::LAYER_ADDED, i++);
        diff->saveLayer(*layer);
        diffs->push_back(diff);
    }
}

void Layers::addLayer(const DefinitionNode &tocopy) {
    auto diff = new LayersDiff(this, LayersDiff::LAYER_ADDED, getLayerCount());
    diff->saveLayer(tocopy);
    addDiff(diff);
}

void Layers::addLayer(const string &name) {
    auto layer = layer_constructor(NULL, name);
    addLayer(*layer);
    delete layer;
}

void Layers::removeLayer(int position) {
    auto diff = new LayersDiff(this, LayersDiff::LAYER_REMOVED, position);
    diff->saveLayer(*getLayer(position));
    addDiff(diff);
}

void Layers::clear() {
    int n = getLayerCount();
    for (int i = n - 1; i >= 0; i--) {
        removeLayer(i);
    }
}
