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

void TexturesRenderer::update()
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
    double total = 1.0 / (mXY + mXZ + mYZ);
    mXY *= total;
    mXZ *= total;
    mYZ *= total;

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
    Vector3 dnorth, deast, dsouth, dwest, normal;

    dnorth = north.sub(center);
    deast = east.sub(center);
    dsouth = south.sub(center);
    dwest = west.sub(center);

    normal = deast.crossProduct(dnorth);
    normal = normal.add(dsouth.crossProduct(deast));
    normal = normal.add(dwest.crossProduct(dsouth));
    normal = normal.add(dnorth.crossProduct(dwest));

    return normal.normalize();
}

static inline Vector3 _getNormal2(Vector3 center, Vector3 east, Vector3 south)
{
    return south.sub(center).crossProduct(east.sub(center)).normalize();
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
    dx = base_normal.crossProduct(pivot).normalize();
    dy = base_normal.crossProduct(dx).normalize();

    /* Apply detail noise locally */
    Vector3 center, north, east, south, west;

    center = base_location.add(base_normal.scale(textures->getTriplanarNoise(layer->_detail_noise, base_location, base_normal)));

    east = base_location.add(dx.scale(offset));
    east = east.add(base_normal.scale(textures->getTriplanarNoise(layer->_detail_noise, east, base_normal)));

    south = base_location.add(dy.scale(offset));
    south = south.add(base_normal.scale(textures->getTriplanarNoise(layer->_detail_noise, south, base_normal)));

    if (renderer->render_quality > 6)
    {
        west = base_location.add(dx.scale(-offset));
        west = west.add(base_normal.scale(textures->getTriplanarNoise(layer->_detail_noise, west, base_normal)));

        north = base_location.add(dy.scale(-offset));
        north = north.add(base_normal.scale(textures->getTriplanarNoise(layer->_detail_noise, north, base_normal)));

        result = _getNormal4(center, north, east, south, west);
    }
    else
    {
        result = _getNormal2(center, east, south);
    }

    if (result.dotProduct(base_normal) < 0.0)
    {
        result = result.scale(-1.0);
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

    return terrain.location.add(terrain.normal.normalize().scale(offset));
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

    // Displacement
    TerrainRenderer::TerrainResult terrain = parent->getTerrainRenderer()->getResult(x, z, 1, 1);

    // TODO Displaced textures had their presence already computed before, store that result and use it

    // Find presence of each layer
    int n = textures->count();
    int start = 0;
    for (int i = 0; i < n; i++)
    {
        TexturesLayerResult &layer = result.layers[i];

        layer.definition = textures->getTextureLayer(i);
        layer.presence = getBasePresence(i, terrain);
        if (layer.presence > 0.9999)
        {
            start = i;
        }
    }
    result.layer_count = n;

    result.base_location = terrain.location;
    result.base_normal = terrain.normal;
    result.final_location = terrain.location;
    result.final_color = COLOR_GREEN;

    // Compute and merge colors of visible layers
    for (int i = start; i < n; i++)
    {
        TexturesLayerResult &layer = result.layers[i];

        if (layer.presence > 0.0)
        {
            Vector3 normal = _getDetailNormal(parent, terrain.location, terrain.normal, layer.definition);
            Vector3 location(x, terrain.location.y, z);
            layer.color = parent->applyLightingToSurface(location, normal, *layer.definition->material);
            layer.color.a = layer.presence;
            result.final_color.mask(layer.color);
        }
        else
        {
            layer.color = COLOR_TRANSPARENT;
        }
    }

    return result;
}
