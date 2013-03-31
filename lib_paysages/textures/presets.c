#include "private.h"

void texturesAutoPreset(TexturesDefinition* definition, TexturesPreset preset)
{
    layersClear(definition->layers);

    if (preset == TEXTURES_PRESET_IRELAND)
    {
    }
    else if (preset == TEXTURES_PRESET_IRELAND)
    {
    }
    else if (preset == TEXTURES_PRESET_IRELAND)
    {
    }
}

void texturesLayerAutoPreset(TexturesLayerDefinition* definition, TexturesLayerPreset preset)
{
    noiseRandomizeOffsets(definition->_displacement_noise);

    switch (preset)
    {
        case TEXTURES_LAYER_PRESET_ROCK:
            break;
        case TEXTURES_LAYER_PRESET_GRASS:
            break;
        case TEXTURES_LAYER_PRESET_SAND:
            break;
        case TEXTURES_LAYER_PRESET_SNOW:
            break;
        default:
            break;
    }

    cloudsLayerValidateDefinition(definition);
}

#if 0
    textures = texturesCreateDefinition();
    layer = layersAddLayer(textures.layers, NULL);
    layersSetName(textures.layers, layer, "Ground");
    texture = layersGetLayer(textures.layers, layer);
    noiseClearLevels(texture->bump_noise);
    noiseRandomizeOffsets(texture->bump_noise);
    noiseAddLevelsSimple(texture->bump_noise, 8, 1.0, -0.5, 0.5, 0.5);
    texture->bump_height = 0.01;
    texture->bump_scaling = 0.045;
    texture->material.base.r = 0.6;
    texture->material.base.g = 0.55;
    texture->material.base.b = 0.57;
    texture->material.reflection = 0.02;
    texture->material.shininess = 3.0;
    texture->thickness = 0.001;
    texture->slope_range = 0.001;
    texture->thickness_transparency = 0.0;
    layer = layersAddLayer(textures.layers, NULL);
    layersSetName(textures.layers, layer, "Grass");
    texture = layersGetLayer(textures.layers, layer);
    zoneAddHeightRangeQuick(texture->zone, 1.0, -6.0, -5.0, 3.0, 15.0);
    zoneAddSlopeRangeQuick(texture->zone, 1.0, 0.0, 0.0, 0.05, 0.4);
    noiseClearLevels(texture->bump_noise);
    noiseRandomizeOffsets(texture->bump_noise);
    noiseAddLevelsSimple(texture->bump_noise, 5, 1.0, -0.2, 0.2, 0.5);
    noiseAddLevelsSimple(texture->bump_noise, 2, 0.03, -0.04, 0.04, 0.5);
    texture->bump_height = 0.002;
    texture->bump_scaling = 0.03;
    texture->material.base.r = 0.12;
    texture->material.base.g = 0.19;
    texture->material.base.b = 0.035;
    texture->material.reflection = 0.003;
    texture->material.shininess = 2.0;
    texture->thickness = 0.02;
    texture->slope_range = 0.03;
    texture->thickness_transparency = 0.005;
    /*texture = texturesGetLayer(&textures, texturesAddLayer(&textures));
    zoneAddHeightRangeQuick(texture->zone, 1.0, 4.0, 5.0, 100.0, 100.0);
    zoneAddSlopeRangeQuick(texture->zone, 1.0, 0.0, 0.0, 0.2, 1.0);
    noiseGenerateBaseNoise(texture->bump_noise, 102400);
    noiseClearLevels(texture->bump_noise);
    noiseAddLevelsSimple(texture->bump_noise, 6, 1.0, 1.0);
    texture->bump_height = 0.002;
    texture->bump_scaling = 0.03;
    texture->material.base.r = 1.0;
    texture->material.base.g = 1.0;
    texture->material.base.b = 1.0;
    texture->material.reflection = 0.25;
    texture->material.shininess = 0.6;
    texture->thickness = 0.05;
    texture->slope_range = 0.3;
    texture->thickness_transparency = 0.015;*/
    scenerySetTextures(&textures);
    texturesDeleteDefinition(&textures);
#endif