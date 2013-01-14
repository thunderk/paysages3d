#include "public.h"
#include "private.h"

/*
 * Terrain presets.
 */

void terrainAutoPreset(TerrainDefinition* definition, TerrainPreset preset)
{
    int resolution = 8;
    switch (preset)
    {
        case TERRAIN_PRESET_STANDARD:
            noiseClearLevels(definition->_height_noise);
            noiseAddLevelsSimple(definition->_height_noise, resolution, pow(2.0, resolution - 1), 25.0);
            noiseSetFunctionParams(definition->_height_noise, NOISE_FUNCTION_SIMPLEX, 0.0);
            definition->scaling = 1.0;
            definition->height = 1.0;
            definition->shadow_smoothing = 0.03;
            break;
        default:
            ;
    }

    TerrainDefinitionClass.validate(definition);
}
