#ifndef VEGETATIONPRESENCEDEFINITION_H
#define VEGETATIONPRESENCEDEFINITION_H

#include "definition_global.h"

#include "DefinitionNode.h"

namespace paysages {
namespace definition {

/**
 * Definition of the presence of vegetation at locations.
 */
class DEFINITIONSHARED_EXPORT VegetationPresenceDefinition: public DefinitionNode
{
public:
    VegetationPresenceDefinition(VegetationLayerDefinition *parent);

    /**
     * Collect instances in a rectangle area.
     *
     * If *outcomers* is true, the rectangle area will be expanded to included potential instances that does not originate
     * in the area, but may have leaves in it.
     *
     * The location vector set in collected instances is based on raw terrain height, without displacement.
     * It's the renderer role to apply the correct displacement.
     */
    bool collectInstances(std::vector<VegetationInstance> *result, const VegetationModelDefinition &model, double xmin, double zmin, double xmax, double zmax, bool outcomers=true) const;

private:
    NoiseNode *noise;
    FloatNode *interval;
};

}
}

#endif // VEGETATIONPRESENCEDEFINITION_H
