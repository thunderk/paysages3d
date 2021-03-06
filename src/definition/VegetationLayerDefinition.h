#pragma once

#include "definition_global.h"

#include "DefinitionNode.h"

namespace paysages {
namespace definition {

/**
 * Definition of one vegetation layer.
 */
class DEFINITIONSHARED_EXPORT VegetationLayerDefinition : public DefinitionNode {
  public:
    VegetationLayerDefinition(DefinitionNode *parent, const string &name);

    inline const VegetationPresenceDefinition *getPresence() const {
        return presence;
    }
    inline const VegetationModelDefinition *getModel() const {
        return model;
    }

    double getMaxHeight() const;

    typedef enum { VEGETATION_BASIC_TREES } VegetationLayerPreset;
    void applyPreset(VegetationLayerPreset preset, RandomGenerator &random = RandomGeneratorDefault);

  private:
    /**
     * Geographic area of presence of this layer.
     */
    VegetationPresenceDefinition *presence;

    /**
     * Base vegetation model to use for this layer.
     */
    VegetationModelDefinition *model;
};
}
}
