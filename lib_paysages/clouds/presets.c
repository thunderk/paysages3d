#include "private.h"

#include <math.h>

/*
 * Clouds presets.
 */

void cloudsAutoPreset(CloudsLayerDefinition* definition, CloudsPreset preset)
{
    definition->material.base.r = 0.7;
    definition->material.base.g = 0.7;
    definition->material.base.b = 0.7;
    definition->material.base.a = 1.0;

    switch (preset)
    {
        case CLOUDS_PRESET_CIRRUS:
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
        case CLOUDS_PRESET_CUMULUS:
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
        case CLOUDS_PRESET_STRATOCUMULUS:
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
        case CLOUDS_PRESET_STRATUS:
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
