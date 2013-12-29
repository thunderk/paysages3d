#include "TexturesDefinition.h"

#include "TextureLayerDefinition.h"

static BaseDefinition* _layer_constructor(Layers* parent)
{
    return new TextureLayerDefinition(parent);
}

TexturesDefinition::TexturesDefinition(BaseDefinition *parent)
    : Layers(parent, _layer_constructor)
{
}

void TexturesDefinition::applyPreset(TexturesPreset preset)
{
    TextureLayerDefinition* layer;
    clear();

    layer = getTextureLayer(addLayer());
    layer->applyPreset(TextureLayerDefinition::TEXTURES_LAYER_PRESET_MUD);
    layer->setName("Mud");

    if (preset == TEXTURES_PRESET_FULL)
    {
        layer = getTextureLayer(addLayer());
        layer->applyPreset(TextureLayerDefinition::TEXTURES_LAYER_PRESET_ROCK);
        layer->setName("Ground");

        layer = getTextureLayer(addLayer());
        layer->applyPreset(TextureLayerDefinition::TEXTURES_LAYER_PRESET_GRASS);
        layer->setName("Grass");

        layer = getTextureLayer(addLayer());
        layer->applyPreset(TextureLayerDefinition::TEXTURES_LAYER_PRESET_SAND);
        layer->setName("Sand");

        layer = getTextureLayer(addLayer());
        layer->applyPreset(TextureLayerDefinition::TEXTURES_LAYER_PRESET_SNOW);
        layer->setName("Snow");
    }
    else if (preset == TEXTURES_PRESET_IRELAND)
    {
        layer = getTextureLayer(addLayer());
        layer->applyPreset(TextureLayerDefinition::TEXTURES_LAYER_PRESET_ROCK);
        layer->setName("Ground");

        layer = getTextureLayer(addLayer());
        layer->applyPreset(TextureLayerDefinition::TEXTURES_LAYER_PRESET_GRASS);
        layer->setName("Grass");
    }
    else if (preset == TEXTURES_PRESET_ALPS)
    {
        layer = getTextureLayer(addLayer());
        layer->applyPreset(TextureLayerDefinition::TEXTURES_LAYER_PRESET_ROCK);
        layer->setName("Ground");

        layer = getTextureLayer(addLayer());
        layer->applyPreset(TextureLayerDefinition::TEXTURES_LAYER_PRESET_SNOW);
        layer->setName("Snow");
    }
    else if (preset == TEXTURES_PRESET_CANYON)
    {
        /* TODO */
    }
}

double TexturesDefinition::getMaximalDisplacement()
{
    double result = 0.0;
    int n = count();
    for (int i = 0; i < n; i++)
    {
        result += getTextureLayer(i)->displacement_height;
    }
    return result;
}
