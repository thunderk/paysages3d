#include "tex_preview.h"
#include "private.h"

#include "RenderingScenery.h"
#include "TexturesDefinition.h"

void TexturesPreviewLayerCoverage_bind(Renderer* renderer, TexturesDefinition* definition)
{
    TerrainRendererClass.bind(renderer, RenderingScenery::getCurrent()->getTerrain());
    TexturesRendererClass.bind(renderer, definition);
}

Color TexturesPreviewLayerCoverage_getColor(Renderer* renderer, double x, double y, double, int layer)
{
    TextureLayerDefinition* layerdef;
    TerrainResult terrain;
    double presence;
    Color result;

    layerdef = renderer->textures->definition->getTextureLayer(layer);
    if (layerdef)
    {
        terrain = renderer->terrain->getResult(renderer, x, y, 1, 1);
        presence = texturesGetLayerBasePresence(layerdef, terrain);

        result.r = result.g = result.b = presence;
        result.a = 1.0;
    }
    else
    {
        result = COLOR_BLACK;
    }

    return result;
}

static double _getPresenceFull(Renderer*, int, TerrainResult)
{
    return 1.0;
}

void TexturesPreviewLayerLook_bind(Renderer* renderer, TexturesDefinition* definition)
{
    TexturesRendererClass.bind(renderer, definition);
    renderer->textures->getBasePresence = _getPresenceFull;
}

Color TexturesPreviewLayerLook_getColor(Renderer* renderer, double x, double y, double, int layer)
{
    TexturesResult result = renderer->textures->applyToTerrain(renderer, x, y);
    if (layer >= 0 && layer < result.layer_count)
    {
        return result.layers[layer].color;
    }
    else
    {
        return COLOR_BLACK;
    }
}


void TexturesPreviewCumul_bind(Renderer* renderer, TexturesDefinition* definition)
{
    TerrainRendererClass.bind(renderer, RenderingScenery::getCurrent()->getTerrain());
    TexturesRendererClass.bind(renderer, definition);
}

Color TexturesPreviewCumul_getColor(Renderer* renderer, double x, double y, double, int)
{
    return renderer->textures->applyToTerrain(renderer, x, y).final_color;
}
