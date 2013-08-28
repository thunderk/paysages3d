#include "tex_preview.h"
#include "private.h"

#include "rendering/scenery.h"
#include "rendering/tools.h"

void TexturesPreviewLayerCoverage_bind(Renderer* renderer, TexturesDefinition* definition)
{
    TerrainDefinition* terrain = (TerrainDefinition*)TerrainDefinitionClass.create();
    sceneryGetTerrain(terrain);
    TerrainRendererClass.bind(renderer, terrain);
    TerrainDefinitionClass.destroy(terrain);

    TexturesRendererClass.bind(renderer, definition);
}

Color TexturesPreviewLayerCoverage_getColor(Renderer* renderer, double x, double y, double scaling, int layer)
{
    UNUSED(scaling);

    TexturesLayerDefinition* layerdef;
    TerrainResult terrain;
    double presence;
    Color result;

    layerdef = (TexturesLayerDefinition*)layersGetLayer(renderer->textures->definition->layers, layer);
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

static double _getPresenceFull(Renderer* renderer, int layer, TerrainResult terrain)
{
    UNUSED(renderer);
    UNUSED(layer);
    UNUSED(terrain);

    return 1.0;
}

void TexturesPreviewLayerLook_bind(Renderer* renderer, TexturesDefinition* definition)
{
    TexturesRendererClass.bind(renderer, definition);
    renderer->textures->getBasePresence = _getPresenceFull;
}

Color TexturesPreviewLayerLook_getColor(Renderer* renderer, double x, double y, double scaling, int layer)
{
    UNUSED(scaling);
    UNUSED(layer);

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
    TerrainDefinition* terrain = (TerrainDefinition*)TerrainDefinitionClass.create();
    sceneryGetTerrain(terrain);
    TerrainRendererClass.bind(renderer, terrain);
    TerrainDefinitionClass.destroy(terrain);

    TexturesRendererClass.bind(renderer, definition);
}

Color TexturesPreviewCumul_getColor(Renderer* renderer, double x, double y, double scaling, int layer)
{
    UNUSED(scaling);
    UNUSED(layer);

    return renderer->textures->applyToTerrain(renderer, x, y).final_color;
}
