#include "TerrainRenderer.h"

#include "SoftwareRenderer.h"
#include "Scenery.h"
#include "TerrainDefinition.h"
#include "TexturesRenderer.h"
#include "LightComponent.h"

TerrainRenderer::TerrainRenderer(SoftwareRenderer* parent):
    parent(parent)
{
}

TerrainRenderer::~TerrainRenderer()
{
}

void TerrainRenderer::update()
{
}

double TerrainRenderer::getHeight(double x, double z, int with_painting)
{
    return parent->getScenery()->getTerrain()->getInterpolatedHeight(x, z, 1, with_painting);
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

TerrainRenderer::TerrainResult TerrainRenderer::getResult(double x, double z, int with_painting, int with_textures)
{
    TerrainResult result;
    double detail = 0.001; /* TODO */

    /* Normal */
    Vector3 center, north, east, south, west;

    center.x = x;
    center.z = z;
    center.y = getHeight(center.x, center.z, with_painting);

    east.x = x + detail;
    east.z = z;
    east.y = getHeight(east.x, east.z, with_painting);

    south.x = x;
    south.z = z + detail;
    south.y = getHeight(south.x, south.z, with_painting);

    if (parent->render_quality > 6)
    {
        west.x = x - detail;
        west.z = z;
        west.y = getHeight(west.x, west.z, with_painting);

        north.x = x;
        north.z = z - detail;
        north.y = getHeight(north.x, north.z, with_painting);

        result.normal = _getNormal4(center, north, east, south, west);
    }
    else
    {
        result.normal = _getNormal2(center, east, south);
    }

    /* Location */
    result.location = center;

    /* Texture displacement */
    if (with_textures)
    {
        center = parent->getTexturesRenderer()->displaceTerrain(result);
        result.location = center;

        /* Recompute normal */
        if (parent->render_quality > 6)
        {
            /* Use 5 points on displaced terrain */
            east = parent->getTexturesRenderer()->displaceTerrain(getResult(east.x, east.z, with_painting, 0));
            south = parent->getTexturesRenderer()->displaceTerrain(getResult(south.x, south.z, with_painting, 0));
            west = parent->getTexturesRenderer()->displaceTerrain(getResult(west.x, west.z, with_painting, 0));
            north = parent->getTexturesRenderer()->displaceTerrain(getResult(north.x, north.z, with_painting, 0));

            result.normal = _getNormal4(center, north, east, south, west);
        }
        else if (parent->render_quality > 2)
        {
            /* Use 3 points on displaced terrain */
            east = parent->getTexturesRenderer()->displaceTerrain(getResult(east.x, east.z, with_painting, 0));
            south = parent->getTexturesRenderer()->displaceTerrain(getResult(south.x, south.z, with_painting, 0));

            result.normal = _getNormal2(center, east, south);
        }
        else
        {
            /* TODO Use texture noise directly, as if terrain was a plane */
        }
    }

    return result;
}

Color TerrainRenderer::getFinalColor(const Vector3 &location, double)
{
    /* TODO Restore precision control */
    TexturesRenderer::TexturesResult textures = parent->getTexturesRenderer()->applyToTerrain(location.x, location.z);
    return parent->applyMediumTraversal(textures.final_location, textures.final_color);
}

RayCastingResult TerrainRenderer::castRay(const Vector3 &start, const Vector3 &direction)
{
    RayCastingResult result;
    TerrainDefinition* definition = parent->getScenery()->getTerrain();
    Vector3 inc_vector, direction_norm, cursor;
    double inc_value, inc_base, inc_factor, height, diff, lastdiff, length;

    cursor = start;
    direction_norm = direction.normalize();
    inc_factor = (double)parent->render_quality;
    inc_base = 1.0;
    inc_value = inc_base / inc_factor;
    lastdiff = start.y - getHeight(start.x, start.z, 1);

    length = 0.0;
    do
    {
        inc_vector = direction_norm.scale(inc_value);
        length += inc_vector.getNorm();
        cursor = cursor.add(inc_vector);
        height = getHeight(cursor.x, cursor.z, 1);
        diff = cursor.y - height;
        if (diff < 0.0)
        {
            if (fabs(diff - lastdiff) > 0.00001)
            {
                cursor = cursor.add(inc_vector.scale(-diff / (diff - lastdiff)));
                cursor.y = getHeight(cursor.x, cursor.z, 1);
            }
            else
            {
                cursor.y = height;
            }
            result.hit = 1;
            result.hit_location = cursor;
            result.hit_color = getFinalColor(cursor, parent->getPrecision(result.hit_location));
            return result;
        }

        if (diff < inc_base / inc_factor)
        {
            inc_value = inc_base / inc_factor;
        }
        else if (diff > inc_base)
        {
            inc_value = inc_base;
        }
        else
        {
            inc_value = diff;
        }
        lastdiff = diff;
    }
    while (length < 50.0 && cursor.y <= definition->_max_height);

    result.hit = 0;
    return result;
}

bool TerrainRenderer::applyLightFilter(LightComponent &light, const Vector3 &at)
{
    TerrainDefinition* definition = parent->getScenery()->getTerrain();
    Vector3 inc_vector, direction_to_light, cursor;
    double inc_value, inc_base, inc_factor, height, diff, light_factor, smoothing, length;

    if (at.y > definition->getHeightInfo().max_height)
    {
        // Location is above terrain, don't bother
        return true;
    }

    direction_to_light = light.direction.scale(-1.0);
    if (direction_to_light.y < -0.05)
    {
        light.color = COLOR_BLACK;
        return false;
    }
    else if (direction_to_light.y < 0.0000)
    {
        light.color.r *= (0.05 + direction_to_light.y) / 0.05;
        light.color.g *= (0.05 + direction_to_light.y) / 0.05;
        light.color.b *= (0.05 + direction_to_light.y) / 0.05;
    }

    cursor = at;
    inc_factor = (double)parent->render_quality;
    inc_base = definition->height / definition->scaling;
    inc_value = inc_base / inc_factor;
    smoothing = definition->shadow_smoothing;

    light_factor = 1.0;
    length = 0.0;
    diff = 0.0;
    do
    {
        inc_vector = direction_to_light.scale(inc_value);
        length += inc_vector.getNorm();
        cursor = cursor.add(inc_vector);
        height = parent->getTerrainRenderer()->getResult(cursor.x, cursor.z, 1, 1).location.y;
        diff = cursor.y - height;
        if (diff < 0.0)
        {
            if (length * smoothing > 0.000001)
            {
                light_factor += diff * inc_vector.getNorm() / (length * smoothing);
            }
            else
            {
                light_factor = 0.0;
            }
        }

        if (diff < inc_base / inc_factor)
        {
            inc_value = inc_base / inc_factor;
        }
        else if (diff > inc_base)
        {
            inc_value = inc_base;
        }
        else
        {
            inc_value = diff;
        }
    }
    while (light_factor > 0.0 && length < (10.0 * inc_factor) && cursor.y <= definition->_max_height);

    if (light_factor <= 0.0)
    {
        light.color = COLOR_BLACK;
        return false;
    }
    else
    {
        light.color.r *= light_factor;
        light.color.g *= light_factor;
        light.color.b *= light_factor;

        return true;
    }
}

double TerrainRenderer::getWaterHeight()
{
    TerrainDefinition* terrain = parent->getScenery()->getTerrain();
    return terrain->water_height * terrain->height * terrain->scaling;
}
