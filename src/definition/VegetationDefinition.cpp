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

double VegetationDefinition::getMaxHeight() const
{
    double max_height = 0.0;
    int n = count();

    for (int i = 0; i < n; i++)
    {
        double layer_height = getVegetationLayer(i)->getMaxHeight();
        if (layer_height > max_height)
        {
            max_height = layer_height;
        }
    }

    return max_height;
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
