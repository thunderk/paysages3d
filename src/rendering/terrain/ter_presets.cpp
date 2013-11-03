#include "private.h"

#include <math.h>
#include "NoiseGenerator.h"

/*
 * Terrain presets.
 */

void terrainAutoPreset(TerrainDefinition* definition, TerrainPreset preset)
{
    int resolution = 8;
    switch (preset)
    {
    case TERRAIN_PRESET_STANDARD:
        definition->_height_noise->randomizeOffsets();
        definition->_height_noise->clearLevels();
        definition->_height_noise->addLevelSimple(pow(2.0, resolution + 1), -1.0, 1.0);
        definition->_height_noise->addLevelsSimple(resolution - 2, pow(2.0, resolution - 1), -0.7, 0.7, 0.5);
        definition->_height_noise->normalizeAmplitude(-1.0, 1.0, 0);
        definition->_height_noise->setFunctionParams(NOISE_FUNCTION_SIMPLEX, 0.0, 0.0);
        definition->scaling = 1.0;
        definition->height = 30.0;
        definition->shadow_smoothing = 0.03;
        break;
    default:
        ;
    }

    terrainClearPainting(definition->height_map);

    TerrainDefinitionClass.validate(definition);
}
