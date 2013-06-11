#include "private.h"

#include <math.h>

/*
 * Terrain presets.
 */

void terrainAutoPreset(TerrainDefinition* definition, TerrainPreset preset)
{
    int resolution = 8;
    switch (preset)
    {
    case TERRAIN_PRESET_STANDARD:
        noiseRandomizeOffsets(definition->_height_noise);
        noiseClearLevels(definition->_height_noise);
        noiseAddLevelSimple(definition->_height_noise, pow(2.0, resolution + 1), -1.0, 1.0);
        noiseAddLevelsSimple(definition->_height_noise, resolution - 2, pow(2.0, resolution - 1), -0.7, 0.7, 0.5);
        noiseNormalizeAmplitude(definition->_height_noise, -1.0, 1.0, 0);
        noiseSetFunctionParams(definition->_height_noise, NOISE_FUNCTION_SIMPLEX, 0.0, 0.0);
        definition->scaling = 1.0;
        definition->height = 30.0;
        definition->shadow_smoothing = 0.03;
        break;
    default:
        ;
    }

    TerrainDefinitionClass.validate(definition);
}
