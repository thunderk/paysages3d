#include "private.h"

#include <cstdlib>
#include "TexturesDefinition.h"
#include "TextureLayerDefinition.h"
#include "../renderer.h"

/******************** Tools ********************/
static inline Vector3 _getNormal4(Vector3 center, Vector3 north, Vector3 east, Vector3 south, Vector3 west)
{
    /* TODO This is duplicated in terrain/main.c */
    Vector3 dnorth, deast, dsouth, dwest, normal;

    dnorth = v3Sub(north, center);
    deast = v3Sub(east, center);
    dsouth = v3Sub(south, center);
    dwest = v3Sub(west, center);

    normal = v3Cross(deast, dnorth);
    normal = v3Add(normal, v3Cross(dsouth, deast));
    normal = v3Add(normal, v3Cross(dwest, dsouth));
    normal = v3Add(normal, v3Cross(dnorth, dwest));

    return v3Normalize(normal);
}

static inline Vector3 _getNormal2(Vector3 center, Vector3 east, Vector3 south)
{
    /* TODO This is duplicated in terrain/main.c */
    return v3Normalize(v3Cross(v3Sub(south, center), v3Sub(east, center)));
}

static Vector3 _getDetailNormal(Renderer* renderer, Vector3 base_location, Vector3 base_normal, TextureLayerDefinition* layer)
{
    Vector3 result;
    double offset = 0.01;

    /* Find guiding vectors in the appoximated local plane */
    Vector3 dx, dy;
    Vector3 pivot;
    if (base_normal.y > 0.95)
    {
        pivot = VECTOR_NORTH;
    }
    else
    {
        pivot = VECTOR_UP;
    }
    dx = v3Normalize(v3Cross(base_normal, pivot));
    dy = v3Cross(base_normal, dx);

    /* Apply detail noise locally */
    Vector3 center, north, east, south, west;

    center = v3Add(base_location, v3Scale(base_normal, texturesGetTriplanarNoise(layer->_detail_noise, base_location, base_normal)));

    east = v3Add(base_location, v3Scale(dx, offset));
    east = v3Add(east, v3Scale(base_normal, texturesGetTriplanarNoise(layer->_detail_noise, east, base_normal)));

    south = v3Add(base_location, v3Scale(dy, offset));
    south = v3Add(south, v3Scale(base_normal, texturesGetTriplanarNoise(layer->_detail_noise, south, base_normal)));

    if (renderer->render_quality > 6)
    {
        west = v3Add(base_location, v3Scale(dx, -offset));
        west = v3Add(west, v3Scale(base_normal, texturesGetTriplanarNoise(layer->_detail_noise, west, base_normal)));

        north = v3Add(base_location, v3Scale(dy, -offset));
        north = v3Add(north, v3Scale(base_normal, texturesGetTriplanarNoise(layer->_detail_noise, north, base_normal)));

        result = _getNormal4(center, north, east, south, west);
    }
    else
    {
        result = _getNormal2(center, east, south);
    }

    if (v3Dot(result, base_normal) < 0.0)
    {
        result = v3Scale(result, -1.0);
    }
    return result;
}

/******************** Real ********************/
static Vector3 _realDisplaceTerrain(Renderer* renderer, TerrainResult terrain)
{
    TexturesDefinition* textures = renderer->textures->definition;
    double offset = 0.0;
    int i, n;

    n = textures->count();
    for (i = 0; i < n; i++)
    {
        TextureLayerDefinition* layer = textures->getTextureLayer(i);

        if (layer->displacement_height > 0.0)
        {
            double presence = texturesGetLayerBasePresence(layer, terrain);
            Vector3 location = {terrain.location.x / layer->displacement_scaling, terrain.location.y / layer->displacement_scaling, terrain.location.z / layer->displacement_scaling};
            offset += texturesGetTriplanarNoise(layer->_displacement_noise, location, terrain.normal) * presence * layer->displacement_height;
        }
    }

    return v3Add(terrain.location, v3Scale(v3Normalize(terrain.normal), offset));
}

static double _realGetBasePresence(Renderer* renderer, int layer, TerrainResult terrain)
{
    TextureLayerDefinition* layerdef = renderer->textures->definition->getTextureLayer(layer);
    return texturesGetLayerBasePresence(layerdef, terrain);
}

static TexturesResult _realApplyToTerrain(Renderer* renderer, double x, double z)
{
    TexturesDefinition* textures = renderer->textures->definition;
    TexturesResult result;

    /* Displacement */
    TerrainResult terrain = renderer->terrain->getResult(renderer, x, z, 1, 1);

    /* TODO Displaced textures had their presence already computed before, store that result and use it */

    /* Find presence of each layer */
    int i, n;
    n = textures->count();
    for (i = 0; i < n; i++)
    {
        TexturesLayerResult* info = result.layers + i;
        info->layer = textures->getTextureLayer(i);
        info->presence = renderer->textures->getBasePresence(renderer, i, terrain);
        if (info->presence > 0.0)
        {
            Vector3 normal = _getDetailNormal(renderer, terrain.location, terrain.normal, info->layer);
            info->color = renderer->applyLightingToSurface(terrain.location, normal, *info->layer->material);
        }
        else
        {
            info->color = COLOR_TRANSPARENT;
        }
    }
    result.layer_count = n;

    result.base_location = terrain.location;
    result.base_normal = terrain.normal;
    result.final_location = terrain.location;
    result.final_color = COLOR_GREEN;
    for (i = 0; i < n; i++)
    {
        if (result.layers[i].presence > 0.0)
        {
            result.layers[i].color.a = result.layers[i].presence;
            colorMask(&result.final_color, &result.layers[i].color);
        }
    }

    return result;
}

/******************** Fake ********************/
static Vector3 _fakeDisplaceTerrain(Renderer*, TerrainResult terrain)
{
    return terrain.location;
}

static double _fakeGetBasePresence(Renderer*, int, TerrainResult)
{
    return 1.0;
}

static TexturesResult _fakeApplyToTerrain(Renderer*, double x, double z)
{
    TexturesResult result;

    result.base_location.x = x;
    result.base_location.y = 0.0;
    result.base_location.z = z;
    result.base_normal = VECTOR_UP;
    result.layer_count = 0;
    result.final_location = result.base_location;
    result.final_color = COLOR_WHITE;

    return result;
}

/******************** Renderer ********************/
static TexturesRenderer* _createRenderer()
{
    TexturesRenderer* result;

    result = new TexturesRenderer;
    result->definition = new TexturesDefinition(NULL);

    result->displaceTerrain = _fakeDisplaceTerrain;
    result->getBasePresence = _fakeGetBasePresence;
    result->applyToTerrain = _fakeApplyToTerrain;

    return result;
}

static void _deleteRenderer(TexturesRenderer* renderer)
{
    delete renderer->definition;
    delete renderer;
}

static void _bindRenderer(Renderer* renderer, TexturesDefinition* definition)
{
    definition->copy(renderer->textures->definition);

    renderer->textures->displaceTerrain = _realDisplaceTerrain;
    renderer->textures->getBasePresence = _realGetBasePresence;
    renderer->textures->applyToTerrain = _realApplyToTerrain;
}

StandardRenderer TexturesRendererClass = {
    (FuncObjectCreate)_createRenderer,
    (FuncObjectDelete)_deleteRenderer,
    (FuncObjectBind)_bindRenderer
};
