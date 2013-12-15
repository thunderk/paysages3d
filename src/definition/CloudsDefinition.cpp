#include "CloudsDefinition.h"

#include "CloudLayerDefinition.h"

static BaseDefinition* _layerConstructor(Layers* parent)
{
    return new CloudLayerDefinition(parent);
}

CloudsDefinition::CloudsDefinition(BaseDefinition* parent):
    Layers(parent, _layerConstructor)
{
    setName("clouds");
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
