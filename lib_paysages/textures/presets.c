#include "private.h"

void texturesAutoPreset(TexturesDefinition* definition, TexturesPreset preset)
{
    TexturesLayerDefinition* layer;
    layersClear(definition->layers);

    if (preset == TEXTURES_PRESET_IRELAND)
    {
        layer = layersGetLayer(definition->layers, layersAddLayer(definition->layers, NULL));
        texturesLayerAutoPreset(layer, TEXTURES_LAYER_PRESET_ROCK);
        layersSetName(definition->layers, 0, "Ground");

        layer = layersGetLayer(definition->layers, layersAddLayer(definition->layers, NULL));
        texturesLayerAutoPreset(layer, TEXTURES_LAYER_PRESET_GRASS);
        layersSetName(definition->layers, 1, "Grass");
    }
    else if (preset == TEXTURES_PRESET_ALPS)
    {
        /* TODO */
    }
    else if (preset == TEXTURES_PRESET_CANYON)
    {
        /* TODO */
    }
}

void texturesLayerAutoPreset(TexturesLayerDefinition* definition, TexturesLayerPreset preset)
{
    noiseRandomizeOffsets(definition->_displacement_noise);
    noiseRandomizeOffsets(definition->_detail_noise);

    zoneClear(definition->terrain_zone);

    switch (preset)
    {
        case TEXTURES_LAYER_PRESET_ROCK:
            definition->displacement_height = 0.1;
            definition->displacement_scaling = 2.0;
            definition->displacement_offset = 0.0;
            definition->material.base.r = 0.6;
            definition->material.base.g = 0.55;
            definition->material.base.b = 0.57;
            definition->material.reflection = 0.02;
            definition->material.shininess = 3.0;
            break;
        case TEXTURES_LAYER_PRESET_GRASS:
            zoneAddHeightRangeQuick(definition->terrain_zone, 1.0, -6.0, -5.0, 3.0, 15.0);
            zoneAddSlopeRangeQuick(definition->terrain_zone, 1.0, 0.0, 0.0, 0.05, 0.4);
            definition->displacement_height = 0.0;
            definition->displacement_scaling = 1.0;
            definition->displacement_offset = 0.0;
            definition->material.base.r = 0.12;
            definition->material.base.g = 0.19;
            definition->material.base.b = 0.035;
            definition->material.reflection = 0.003;
            definition->material.shininess = 2.0;
            break;
        case TEXTURES_LAYER_PRESET_SAND:
            zoneAddHeightRangeQuick(definition->terrain_zone, 1.0, -5.5, -4.5, -4.0, -3.0);
            zoneAddSlopeRangeQuick(definition->terrain_zone, 1.0, 0.0, 0.0, 0.15, 0.3);
            definition->displacement_height = 0.0;
            definition->displacement_scaling = 1.0;
            definition->displacement_offset = 0.0;
            definition->material.base.r = 0.30;
            definition->material.base.g = 0.28;
            definition->material.base.b = 0.02;
            definition->material.reflection = 0.008;
            definition->material.shininess = 6.0;
            break;
        case TEXTURES_LAYER_PRESET_SNOW:
            zoneAddHeightRangeQuick(definition->terrain_zone, 1.0, 4.0, 5.0, 100.0, 100.0);
            zoneAddSlopeRangeQuick(definition->terrain_zone, 1.0, 0.0, 0.0, 0.2, 1.0);
            definition->displacement_height = 0.0;
            definition->displacement_scaling = 1.0;
            definition->displacement_offset = 0.0;
            definition->material.base.r = 1.0;
            definition->material.base.g = 1.0;
            definition->material.base.b = 1.0;
            definition->material.reflection = 0.25;
            definition->material.shininess = 0.6;
            break;
        default:
            break;
    }

    texturesGetLayerType().callback_validate(definition);
}
