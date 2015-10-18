#include "VegetationLayerDefinition.h"

#include "VegetationModelDefinition.h"
#include "VegetationPresenceDefinition.h"

VegetationLayerDefinition::VegetationLayerDefinition(DefinitionNode* parent) :
    DefinitionNode(parent, "layer")
{
    model = new VegetationModelDefinition(this);
    presence = new VegetationPresenceDefinition(this);
}

void VegetationLayerDefinition::applyPreset(VegetationLayerPreset preset)
{
    model->randomize();
}
