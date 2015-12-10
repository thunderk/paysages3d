#include "VegetationDefinition.h"

#include "VegetationLayerDefinition.h"
#include "VegetationModelDefinition.h"

static DefinitionNode *_layer_constructor(Layers *parent, const std::string &name) {
    return new VegetationLayerDefinition(parent, name);
}

VegetationDefinition::VegetationDefinition(DefinitionNode *parent) : Layers(parent, "vegetation", _layer_constructor) {
}

double VegetationDefinition::getMaxHeight() const {
    double max_height = 0.0;
    int n = getLayerCount();

    for (int i = 0; i < n; i++) {
        double layer_height = getVegetationLayer(i)->getMaxHeight();
        if (layer_height > max_height) {
            max_height = layer_height;
        }
    }

    return max_height;
}

void VegetationDefinition::applyPreset(VegetationPreset preset, RandomGenerator &random) {
    VegetationLayerDefinition layer(this, "temp");

    clear();

    if (preset == VEGETATION_PRESET_TEMPERATE) {
        layer.applyPreset(VegetationLayerDefinition::VEGETATION_BASIC_TREES, random);
        layer.setName("Basic tree");
        addLayer(layer);
    }
}
