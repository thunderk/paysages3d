#ifndef LAYERSDIFF_H
#define LAYERSDIFF_H

#include "definition_global.h"

#include "DefinitionDiff.h"

namespace paysages {
namespace definition {

class DEFINITIONSHARED_EXPORT LayersDiff : public DefinitionDiff {
  public:
    typedef enum { LAYER_ADDED, LAYER_REMOVED } LayersDiffOp;

  public:
    LayersDiff(const Layers *layers, LayersDiffOp op, int layer1);
    virtual ~LayersDiff();

    inline LayersDiffOp getOp() const {
        return op;
    }
    inline int getLayer1() const {
        return layer1;
    }

    /**
     * Save a layer state into the diff.
     */
    void saveLayer(const DefinitionNode &src);

    /**
     * Restore the saved layer into a node.
     */
    void restoreSavedLayer(DefinitionNode *dest) const;

  private:
    // Operation to apply
    LayersDiffOp op;
    // Position of first layer changed
    int layer1;
    // Serialized layer data
    PackStream *saved;
};
}
}

#endif // LAYERSDIFF_H
