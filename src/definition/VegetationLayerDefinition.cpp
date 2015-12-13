#include "VegetationLayerDefinition.h"

#include "VegetationModelDefinition.h"
#include "VegetationPresenceDefinition.h"

VegetationLayerDefinition::VegetationLayerDefinition(DefinitionNode *parent, const string &name)
    : DefinitionNode(parent, name, "vegetationlayer") {
    model = new VegetationModelDefinition(this);
    presence = new VegetationPresenceDefinition(this);
}

double VegetationLayerDefinition::getMaxHeight() const {
    return presence->getMaxHeight();
}

void VegetationLayerDefinition::applyPreset(VegetationLayerPreset, RandomGenerator &random) {
    model->randomize(random);
}
