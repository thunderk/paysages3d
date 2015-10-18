#include "VegetationDefinition.h"

#include "VegetationLayerDefinition.h"
#include "VegetationModelDefinition.h"

static DefinitionNode* _layer_constructor(Layers* parent)
{
    return new VegetationLayerDefinition(parent);
}

VegetationDefinition::VegetationDefinition(DefinitionNode* parent) :
    Layers(parent, "vegetation", _layer_constructor)
{
}

void VegetationDefinition::applyPreset(VegetationPreset preset)
{
    VegetationLayerDefinition *layer;

    clear();

    if (preset == VEGETATION_PRESET_TEMPERATE)
    {
        layer = getVegetationLayer(addLayer());
        layer->applyPreset(VegetationLayerDefinition::VEGETATION_BASIC_TREES);
        layer->setName("Basic tree");
    }
}
