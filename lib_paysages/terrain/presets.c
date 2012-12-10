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
            noiseAddLevelsSimple(definition->_height_noise, 8, 12.8, 12.8); /* Detail = 0.1 */
            noiseSetFunctionParams(definition->_height_noise, NOISE_FUNCTION_SIMPLEX, 0.0);
            definition->height = 2.0;
            definition->scaling = 10.0;
            definition->shadow_smoothing = 0.03;
            break;
        default:
            ;
    }

    TerrainDefinitionClass.validate(definition);
}
