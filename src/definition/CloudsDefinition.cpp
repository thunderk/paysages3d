#include "CloudsDefinition.h"

#include "CloudLayerDefinition.h"

static DefinitionNode *_layerConstructor(Layers *parent, const std::string &name) {
    return new CloudLayerDefinition(parent, name);
}

CloudsDefinition::CloudsDefinition(DefinitionNode *parent) : Layers(parent, "clouds", _layerConstructor) {
}

void CloudsDefinition::applyPreset(CloudsPreset preset) {
    clear();

    if (preset == CLOUDS_PRESET_PARTLY_CLOUDY) {
        CloudLayerDefinition layer(NULL, "Strato-cumulus");
        layer.type = CloudLayerDefinition::STRATOCUMULUS;
        addLayer(layer);
    }
}
