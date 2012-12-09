#include "public.h"
#include "private.h"

/*
 * Terrain presets.
 */

void terrainAutoPreset(TerrainDefinition* definition, TerrainPreset preset)
{
    switch (preset)
    {
        case TERRAIN_PRESET_STANDARD:
            noiseClearLevels(definition->_height_noise);
            noiseAddLevelsSimple(definition->_height_noise, 10, 1.0, 1.0);
            noiseSetFunctionParams(definition->_height_noise, NOISE_FUNCTION_SIMPLEX, -0.2);
            definition->height = 12.0 / noiseGetMaxValue(definition->_height_noise);
            definition->scaling = 80.0;
            definition->shadow_smoothing = 0.03;
            break;
        default:
            ;
    }

    TerrainDefinitionClass.validate(definition);
}
