#include "CloudsDefinition.h"

#include "CloudLayerDefinition.h"

static DefinitionNode* _layerConstructor(Layers* parent)
{
    return new CloudLayerDefinition(parent);
}

CloudsDefinition::CloudsDefinition(DefinitionNode* parent):
    Layers(parent, "clouds", _layerConstructor)
{
}

void CloudsDefinition::applyPreset(CloudsPreset preset)
{
    clear();

    if (preset == CLOUDS_PRESET_PARTLY_CLOUDY)
    {
        CloudLayerDefinition* layer = new CloudLayerDefinition(this);
        layer->type = CloudLayerDefinition::STRATOCUMULUS;
        layer->setName("Strato-cumulus");
        addLayer(layer);
    }
}
