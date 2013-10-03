#include "private.h"

void texturesAutoPreset(TexturesDefinition* definition, TexturesPreset preset)
{
    TexturesLayerDefinition* layer;
    layersClear(definition->layers);

    layer = layersGetLayer(definition->layers, layersAddLayer(definition->layers, NULL));
    texturesLayerAutoPreset(layer, TEXTURES_LAYER_PRESET_MUD);
    layersSetName(definition->layers, 0, "Mud");

    if (preset == TEXTURES_PRESET_FULL)
    {
        layer = layersGetLayer(definition->layers, layersAddLayer(definition->layers, NULL));
        texturesLayerAutoPreset(layer, TEXTURES_LAYER_PRESET_ROCK);
        layersSetName(definition->layers, 1, "Ground");

        layer = layersGetLayer(definition->layers, layersAddLayer(definition->layers, NULL));
        texturesLayerAutoPreset(layer, TEXTURES_LAYER_PRESET_GRASS);
        layersSetName(definition->layers, 2, "Grass");

        layer = layersGetLayer(definition->layers, layersAddLayer(definition->layers, NULL));
        texturesLayerAutoPreset(layer, TEXTURES_LAYER_PRESET_SAND);
        layersSetName(definition->layers, 3, "Sand");

        layer = layersGetLayer(definition->layers, layersAddLayer(definition->layers, NULL));
        texturesLayerAutoPreset(layer, TEXTURES_LAYER_PRESET_SNOW);
        layersSetName(definition->layers, 4, "Snow");
    }
    else if (preset == TEXTURES_PRESET_IRELAND)
    {
        layer = layersGetLayer(definition->layers, layersAddLayer(definition->layers, NULL));
        texturesLayerAutoPreset(layer, TEXTURES_LAYER_PRESET_ROCK);
        layersSetName(definition->layers, 1, "Ground");

        layer = layersGetLayer(definition->layers, layersAddLayer(definition->layers, NULL));
        texturesLayerAutoPreset(layer, TEXTURES_LAYER_PRESET_GRASS);
        layersSetName(definition->layers, 2, "Grass");
    }
    else if (preset == TEXTURES_PRESET_ALPS)
    {
        layer = layersGetLayer(definition->layers, layersAddLayer(definition->layers, NULL));
        texturesLayerAutoPreset(layer, TEXTURES_LAYER_PRESET_ROCK);
        layersSetName(definition->layers, 1, "Ground");

        layer = layersGetLayer(definition->layers, layersAddLayer(definition->layers, NULL));
        texturesLayerAutoPreset(layer, TEXTURES_LAYER_PRESET_SNOW);
        layersSetName(definition->layers, 2, "Snow");
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
        case TEXTURES_LAYER_PRESET_MUD:
            definition->displacement_height = 0.05;
            definition->displacement_scaling = 3.0;
            definition->displacement_offset = 0.0;
            definition->material.base = colorToHSL(colorFromValues(0.4, 0.38, 0.35, 1.0));
            definition->material.reflection = 0.003;
            definition->material.shininess = 4.0;
            break;
        case TEXTURES_LAYER_PRESET_ROCK:
            zoneAddHeightRangeQuick(definition->terrain_zone, 1.0, 0.5, 0.55, 1.0, 1.0);
            definition->displacement_height = 0.3;
            definition->displacement_scaling = 2.0;
            definition->displacement_offset = 0.0;
            definition->material.base = colorToHSL(colorFromValues(0.6, 0.55, 0.57, 1.0));
            definition->material.reflection = 0.006;
            definition->material.shininess = 6.0;
            break;
        case TEXTURES_LAYER_PRESET_GRASS:
            zoneAddHeightRangeQuick(definition->terrain_zone, 1.0, 0.45, 0.5, 0.8, 1.0);
            zoneAddSlopeRangeQuick(definition->terrain_zone, 1.0, 0.0, 0.0, 0.05, 0.4);
            definition->displacement_height = 0.0;
            definition->displacement_scaling = 1.0;
            definition->displacement_offset = 0.0;
            definition->material.base = colorToHSL(colorFromValues(0.12, 0.19, 0.035, 1.0));
            definition->material.reflection = 0.001;
            definition->material.shininess = 4.0;
            break;
        case TEXTURES_LAYER_PRESET_SAND:
            zoneAddHeightRangeQuick(definition->terrain_zone, 1.0, 0.498, 0.502, 0.505, 0.51);
            zoneAddSlopeRangeQuick(definition->terrain_zone, 1.0, 0.0, 0.0, 0.7, 0.1);
            definition->displacement_height = 0.02;
            definition->displacement_scaling = 5.0;
            definition->displacement_offset = 0.0;
            definition->material.base = colorToHSL(colorFromValues(0.93, 0.9, 0.8, 1.0));
            definition->material.reflection = 0.008;
            definition->material.shininess = 1.0;
            break;
        case TEXTURES_LAYER_PRESET_SNOW:
            zoneAddHeightRangeQuick(definition->terrain_zone, 1.0, 0.77, 0.85, 1.0, 1.0);
            zoneAddSlopeRangeQuick(definition->terrain_zone, 1.0, 0.0, 0.0, 0.2, 1.0);
            definition->displacement_height = 0.1;
            definition->displacement_scaling = 1.0;
            definition->displacement_offset = 0.0;
            definition->material.base = colorToHSL(colorFromValues(1.0, 1.0, 1.0, 1.0));
            definition->material.reflection = 0.25;
            definition->material.shininess = 0.6;
            break;
        default:
            break;
    }

    texturesGetLayerType().callback_validate(definition);
}
