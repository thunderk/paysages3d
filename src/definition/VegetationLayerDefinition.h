#ifndef VEGETATIONLAYERDEFINITION_H
#define VEGETATIONLAYERDEFINITION_H

#include "definition_global.h"

#include "DefinitionNode.h"

namespace paysages {
namespace definition {

/**
 * Definition of one vegetation layer.
 */
class DEFINITIONSHARED_EXPORT VegetationLayerDefinition : public DefinitionNode
{
public:
    VegetationLayerDefinition(DefinitionNode *parent);
    virtual ~VegetationLayerDefinition();

    virtual void save(PackStream *stream) const override;
    virtual void load(PackStream *stream) override;
    virtual void copy(DefinitionNode *destination) const override;
    virtual void validate() override;

    typedef enum
    {
        VEGETATION_BASIC_TREES
    } VegetationLayerPreset;
    void applyPreset(VegetationLayerPreset preset);

private:
    /**
     * Geographic area of presence of this layer.
     */
    TerrainHeightMap *area;

    /**
     * Base vegetation model to use for this layer.
     */
    VegetationModelDefinition *model;
};

}
}

#endif // VEGETATIONLAYERDEFINITION_H
