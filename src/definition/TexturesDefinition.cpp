#include "TexturesDefinition.h"

#include "TextureLayerDefinition.h"

static DefinitionNode *_layer_constructor(Layers *parent, const std::string &name) {
    return new TextureLayerDefinition(parent, name);
}

TexturesDefinition::TexturesDefinition(DefinitionNode *parent) : Layers(parent, "textures", _layer_constructor) {
}

void TexturesDefinition::applyPreset(TexturesPreset preset, RandomGenerator &random) {
    TextureLayerDefinition layer(NULL, "temp");
    clear();

    layer.applyPreset(TextureLayerDefinition::TEXTURES_LAYER_PRESET_MUD, random);
    layer.setName("Mud");
    addLayer(layer);

    if (preset == TEXTURES_PRESET_FULL) {
        layer.applyPreset(TextureLayerDefinition::TEXTURES_LAYER_PRESET_ROCK, random);
        layer.setName("Ground");
        addLayer(layer);

        layer.applyPreset(TextureLayerDefinition::TEXTURES_LAYER_PRESET_GRASS, random);
        layer.setName("Grass");
        addLayer(layer);

        layer.applyPreset(TextureLayerDefinition::TEXTURES_LAYER_PRESET_SAND, random);
        layer.setName("Sand");
        addLayer(layer);

        layer.applyPreset(TextureLayerDefinition::TEXTURES_LAYER_PRESET_SNOW, random);
        layer.setName("Snow");
        addLayer(layer);
    } else if (preset == TEXTURES_PRESET_IRELAND) {
        layer.applyPreset(TextureLayerDefinition::TEXTURES_LAYER_PRESET_ROCK, random);
        layer.setName("Ground");
        addLayer(layer);

        layer.applyPreset(TextureLayerDefinition::TEXTURES_LAYER_PRESET_GRASS, random);
        layer.setName("Grass");
        addLayer(layer);
    } else if (preset == TEXTURES_PRESET_ALPS) {
        layer.applyPreset(TextureLayerDefinition::TEXTURES_LAYER_PRESET_ROCK, random);
        layer.setName("Ground");
        addLayer(layer);

        layer.applyPreset(TextureLayerDefinition::TEXTURES_LAYER_PRESET_SNOW, random);
        layer.setName("Snow");
        addLayer(layer);
    } else if (preset == TEXTURES_PRESET_CANYON) {
        /* TODO */
    }
}

double TexturesDefinition::getMaximalDisplacement() {
    double result = 0.0;
    int n = getLayerCount();
    for (int i = 0; i < n; i++) {
        result += getTextureLayer(i)->displacement_height;
    }
    return result;
}
