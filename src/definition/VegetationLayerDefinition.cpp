#include "VegetationLayerDefinition.h"

#include "VegetationModelDefinition.h"
#include "VegetationPresenceDefinition.h"

VegetationLayerDefinition::VegetationLayerDefinition(DefinitionNode *parent) : DefinitionNode(parent, "layer") {
    model = new VegetationModelDefinition(this);
    presence = new VegetationPresenceDefinition(this);
}

double VegetationLayerDefinition::getMaxHeight() const {
    return presence->getMaxHeight();
}

void VegetationLayerDefinition::applyPreset(VegetationLayerPreset) {
    model->randomize();
}
