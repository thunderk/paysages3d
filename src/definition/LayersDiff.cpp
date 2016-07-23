#include "LayersDiff.h"

#include "Layers.h"
#include "Logs.h"
#include "PackStream.h"

LayersDiff::LayersDiff(const Layers *layers, LayersDiffOp op, int layer1)
    : DefinitionDiff(layers), op(op), layer1(layer1), saved(NULL) {
}

LayersDiff::LayersDiff(const LayersDiff *other, LayersDiff::LayersDiffOp op, int layer1)
    : DefinitionDiff(other), op(op), layer1(layer1), saved(NULL) {
}

LayersDiff::~LayersDiff() {
    if (saved) {
        delete saved;
    }
}

void LayersDiff::saveLayer(const DefinitionNode &src) {
    if (saved) {
        delete saved;
    }
    saved = new PackStream();
    saved->write(src.getName());
    src.save(saved);
}

void LayersDiff::restoreSavedLayer(DefinitionNode *dest) const {
    if (saved) {
        PackStream reader(saved);
        dest->setName(reader.readString());
        dest->load(&reader);
    }
}

DefinitionDiff *LayersDiff::newReversed() const {
    LayersDiff *result;
    if (op == LAYER_ADDED) {
        result = new LayersDiff(this, LAYER_REMOVED, layer1);
    } else {
        result = new LayersDiff(this, LAYER_ADDED, layer1);
    }
    if (saved) {
        result->saved = new PackStream();
        result->saved->writeFromBuffer(saved);
    }
    return result;
}
