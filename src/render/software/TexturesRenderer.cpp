#include "TexturesRenderer.h"

#include "Scenery.h"
#include "SoftwareRenderer.h"
#include "TextureLayerDefinition.h"
#include "TexturesDefinition.h"
#include "Zone.h"
#include "NoiseGenerator.h"

TexturesRenderer::TexturesRenderer(SoftwareRenderer *parent):
    parent(parent)
{
}

TexturesRenderer::~TexturesRenderer()
{
}

/*
 * Get the base presence factor of a layer, not accounting for other layers.
 */
double TexturesRenderer::getLayerBasePresence(TextureLayerDefinition *layer, const TerrainRenderer::TerrainResult &terrain)
{
    return layer->terrain_zone->getValue(terrain.location, terrain.normal);
}

/*
 * Get triplanar noise value, depending on the normal direction.
 */
double TexturesRenderer::getTriplanarNoise(NoiseGenerator *noise, const Vector3 &location, const Vector3 &normal)
{
    double noiseXY = noise->get2DTotal(location.x, location.y);
    double noiseXZ = noise->get2DTotal(location.x, location.z);
    double noiseYZ = noise->get2DTotal(location.y, location.z);

    double mXY = fabs(normal.z);
    double mXZ = fabs(normal.y);
    double mYZ = fabs(normal.x);
    double total = mXY + mXZ + mYZ;
    mXY /= total;
    mXZ /= total;
    mYZ /= total;

    return noiseXY * mXY + noiseXZ * mXZ + noiseYZ * mYZ;
}

double TexturesRenderer::getMaximalDisplacement(TexturesDefinition *textures)
{
    int i, n;
    double disp = 0.0;
    n = textures->count();
    for (i = 0; i < n; i++)
    {
        TextureLayerDefinition* layer = textures->getTextureLayer(i);

        if (layer->displacement_height > 0.0)
        {
            disp += layer->displacement_height;
        }
    }
    return disp;
}

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

static Vector3 _getDetailNormal(SoftwareRenderer* renderer, Vector3 base_location, Vector3 base_normal, TextureLayerDefinition* layer)
{
    TexturesRenderer* textures = renderer->getTexturesRenderer();
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

    center = v3Add(base_location, v3Scale(base_normal, textures->getTriplanarNoise(layer->_detail_noise, base_location, base_normal)));

    east = v3Add(base_location, v3Scale(dx, offset));
    east = v3Add(east, v3Scale(base_normal, textures->getTriplanarNoise(layer->_detail_noise, east, base_normal)));

    south = v3Add(base_location, v3Scale(dy, offset));
    south = v3Add(south, v3Scale(base_normal, textures->getTriplanarNoise(layer->_detail_noise, south, base_normal)));

    if (renderer->render_quality > 6)
    {
        west = v3Add(base_location, v3Scale(dx, -offset));
        west = v3Add(west, v3Scale(base_normal, textures->getTriplanarNoise(layer->_detail_noise, west, base_normal)));

        north = v3Add(base_location, v3Scale(dy, -offset));
        north = v3Add(north, v3Scale(base_normal, textures->getTriplanarNoise(layer->_detail_noise, north, base_normal)));

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

Vector3 TexturesRenderer::displaceTerrain(const TerrainRenderer::TerrainResult &terrain)
{
    TexturesDefinition* textures = parent->getScenery()->getTextures();
    double offset = 0.0;
    int i, n;

    n = textures->count();
    for (i = 0; i < n; i++)
    {
        TextureLayerDefinition* layer = textures->getTextureLayer(i);

        if (layer->displacement_height > 0.0)
        {
            double presence = getLayerBasePresence(layer, terrain);
            Vector3 location = {terrain.location.x / layer->displacement_scaling, terrain.location.y / layer->displacement_scaling, terrain.location.z / layer->displacement_scaling};
            offset += getTriplanarNoise(layer->_displacement_noise, location, terrain.normal) * presence * layer->displacement_height;
        }
    }

    return v3Add(terrain.location, v3Scale(v3Normalize(terrain.normal), offset));
}

double TexturesRenderer::getBasePresence(int layer, const TerrainRenderer::TerrainResult &terrain)
{
    TextureLayerDefinition* layerdef = parent->getScenery()->getTextures()->getTextureLayer(layer);
    return getLayerBasePresence(layerdef, terrain);
}

TexturesRenderer::TexturesResult TexturesRenderer::applyToTerrain(double x, double z)
{
    TexturesDefinition* textures = parent->getScenery()->getTextures();
    TexturesResult result;

    /* Displacement */
    TerrainRenderer::TerrainResult terrain = parent->getTerrainRenderer()->getResult(x, z, 1, 1);

    /* TODO Displaced textures had their presence already computed before, store that result and use it */

    /* Find presence of each layer */
    int i, n;
    n = textures->count();
    for (i = 0; i < n; i++)
    {
        TexturesLayerResult* info = result.layers + i;
        info->layer = textures->getTextureLayer(i);
        info->presence = getBasePresence(i, terrain);
        if (info->presence > 0.0)
        {
            Vector3 normal = _getDetailNormal(parent, terrain.location, terrain.normal, info->layer);
            info->color = parent->applyLightingToSurface(terrain.location, normal, *info->layer->material);
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
