#include "VegetationPresenceDefinition.h"

#include "Scenery.h"
#include "TerrainDefinition.h"
#include "VegetationLayerDefinition.h"
#include "VegetationInstance.h"

VegetationPresenceDefinition::VegetationPresenceDefinition(VegetationLayerDefinition *parent):
    DefinitionNode(parent, "presence")
{
}

bool VegetationPresenceDefinition::collectInstances(std::vector<VegetationInstance> *result, const VegetationModelDefinition &model, double xmin, double zmin, double xmax, double zmax, bool outcomers) const
{
    // TEMP debug implementation
    double x = (xmin + xmax) / 2.0;
    double z = (zmax + zmin) / 2.0;
    double y = getScenery()->getTerrain()->getInterpolatedHeight(x, z, true, true);
    result->push_back(VegetationInstance(model, Vector3(x, y, z), 0.2));
    return true;
}
