#include "tex_preview.h"
#include "private.h"

#include "rendering/scenery.h"
#include "rendering/tools.h"

void TexturesPreviewLayerCoverage_bind(Renderer* renderer, TexturesDefinition* definition)
{
    TerrainDefinition* terrain = (TerrainDefinition*)TerrainDefinitionClass.create();
    sceneryGetTerrain(renderer->terrain->definition);
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
