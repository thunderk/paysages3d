#include "LayersDiff.h"

#include "PackStream.h"
#include "Layers.h"
#include "Logs.h"

LayersDiff::LayersDiff(const Layers *layers, LayersDiffOp op, int layer1)
    : DefinitionDiff(layers), op(op), layer1(layer1), saved(NULL) {
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
