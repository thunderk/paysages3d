#include "private.h"

#include <math.h>
#include <stdlib.h>

/*
 * Clouds presets.
 */

void cloudsAutoPreset(CloudsDefinition* definition, CloudsPreset preset)
{
    int layer;

    layersClear(definition->layers);

    if (preset == CLOUDS_PRESET_PARTLY_CLOUDY)
    {
        layer = layersAddLayer(definition->layers, NULL);
        cloudsLayerAutoPreset(layersGetLayer(definition->layers, layer), CLOUDS_LAYER_PRESET_CIRRUS);
    }
}

void cloudsLayerAutoPreset(CloudsLayerDefinition* definition, CloudsLayerPreset preset)
{
    noiseRandomizeOffsets(definition->_coverage_noise);
    noiseRandomizeOffsets(definition->_edge_noise);
    noiseRandomizeOffsets(definition->_shape_noise);

    definition->material.base = colorToHSL(colorFromValues(0.7, 0.7, 0.7, 1.0));

    switch (preset)
    {
        case CLOUDS_LAYER_PRESET_CIRRUS:
            definition->type = CLOUDS_TYPE_CIRRUS;
            definition->lower_altitude = 25.0;
            definition->thickness = 2.0;
            definition->material.reflection = 0.4;
            definition->material.shininess = 0.5;
            definition->hardness = 0.0;
            definition->transparencydepth = 3.0;
            definition->lighttraversal = 10.0;
            definition->minimumlight = 0.6;
            definition->shape_scaling = 8.0;
            definition->edge_scaling = 2.0;
            definition->edge_length = 0.8;
            definition->base_coverage = 0.6;
            break;
        case CLOUDS_LAYER_PRESET_CUMULUS:
            definition->type = CLOUDS_TYPE_CUMULUS;
            definition->lower_altitude = 15.0;
            definition->thickness = 15.0;
            definition->material.reflection = 0.5;
            definition->material.shininess = 1.2;
            definition->hardness = 0.25;
            definition->transparencydepth = 1.5;
            definition->lighttraversal = 8.0;
            definition->minimumlight = 0.4;
            definition->shape_scaling = 20.0;
            definition->edge_scaling = 2.0;
            definition->edge_length = 0.0;
            definition->base_coverage = 0.7;
            break;
        case CLOUDS_LAYER_PRESET_STRATOCUMULUS:
            definition->type = CLOUDS_TYPE_STRATOCUMULUS;
            definition->lower_altitude = 5.0;
            definition->thickness = 6.0;
            definition->material.reflection = 0.3;
            definition->material.shininess = 0.8;
            definition->hardness = 0.25;
            definition->transparencydepth = 1.5;
            definition->lighttraversal = 7.0;
            definition->minimumlight = 0.4;
            definition->shape_scaling = 10.0;
            definition->edge_scaling = 0.8;
            definition->edge_length = 0.3;
            definition->base_coverage = 0.4;
            break;
        case CLOUDS_LAYER_PRESET_STRATUS:
            definition->type = CLOUDS_TYPE_STRATUS;
            definition->lower_altitude = 3.0;
            definition->thickness = 4.0;
            definition->material.reflection = 0.1;
            definition->material.shininess = 0.8;
            definition->hardness = 0.1;
            definition->transparencydepth = 3.0;
            definition->lighttraversal = 10.0;
            definition->minimumlight = 0.6;
            definition->shape_scaling = 8.0;
            definition->edge_scaling = 2.0;
            definition->edge_length = 1.0;
            definition->base_coverage = 0.4;
            break;
        default:
            break;
    }

    cloudsLayerValidateDefinition(definition);
}
