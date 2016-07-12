#pragma once

#include "definition_global.h"

#include "Layers.h"

namespace paysages {
namespace definition {

/**
 * Definition of all vegetation layers in the scenery.
 */
class DEFINITIONSHARED_EXPORT VegetationDefinition : public Layers {
  public:
    VegetationDefinition(DefinitionNode *parent);

    /**
     * Get a vegetation layer by its position.
     */
    inline VegetationLayerDefinition *getVegetationLayer(int position) const {
        return (VegetationLayerDefinition *)getLayer(position);
    }

    /**
     * Get the max height of all layers assembled.
     */
    double getMaxHeight() const;

    typedef enum { VEGETATION_PRESET_TEMPERATE } VegetationPreset;
    void applyPreset(VegetationPreset preset, RandomGenerator &random = RandomGeneratorDefault);
};
}
}
