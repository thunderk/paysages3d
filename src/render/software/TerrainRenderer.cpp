#include "TerrainRenderer.h"

#include "SoftwareRenderer.h"
#include "Scenery.h"
#include "TerrainDefinition.h"
#include "TexturesRenderer.h"
#include "LightComponent.h"
#include "TerrainRayWalker.h"
#include "RayCastingResult.h"

TerrainRenderer::TerrainRenderer(SoftwareRenderer* parent):
    parent(parent)
{
    walker_ray = new TerrainRayWalker(parent);
    walker_shadows = new TerrainRayWalker(parent);
    quad_normals = false;
}

TerrainRenderer::~TerrainRenderer()
{
    delete walker_ray;
    delete walker_shadows;
}

void TerrainRenderer::update()
{
    walker_ray->update();
    walker_shadows->update();
}

void TerrainRenderer::setQuality(bool quad_normals, double ray_precision, double shadow_precision)
{
    this->quad_normals = quad_normals;

    walker_ray->setQuality(ray_precision);
    walker_shadows->setQuality(shadow_precision);
}

void TerrainRenderer::setQuality(double factor)
{
    setQuality(factor > 0.6, factor, factor * factor);
}

double TerrainRenderer::getHeight(double x, double z, bool with_painting, bool water_offset)
{
    return parent->getScenery()->getTerrain()->getInterpolatedHeight(x, z, true, with_painting, water_offset);
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

TerrainRenderer::TerrainResult TerrainRenderer::getResult(double x, double z, bool with_painting, bool with_textures)
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
    TerrainRayWalker::TerrainHitResult walk_result;
    if (walker_ray->startWalking(start, direction.normalize(), 0.0, walk_result))
    {
        result.hit = true;
        result.hit_location = walk_result.hit_location;
        result.hit_color = getFinalColor(walk_result.hit_location, parent->getPrecision(walk_result.hit_location));
    }
    else
    {
        result.hit = false;
    }
    return result;
}

bool TerrainRenderer::applyLightFilter(LightComponent &light, const Vector3 &at)
{
    TerrainDefinition* definition = parent->getScenery()->getTerrain();
    TerrainRayWalker::TerrainHitResult walk_result;

    // If location is above terrain, don't bother
    if (at.y > definition->getHeightInfo().max_height)
    {
        return true;
    }

    // Handle sun below horizon
    Vector3 direction_to_light = light.direction.scale(-1.0);
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

    // Walk to find an intersection
    double escape_angle = definition->shadow_smoothing;
    // TODO max length should depend on the sun light angle and altitude range
    if (walker_shadows->startWalking(at, direction_to_light, escape_angle, walk_result))
    {
        if (walk_result.escape_angle == 0.0)
        {
            // Hit, with no escape, cancelling the light
            light.color = COLOR_BLACK;
            return false;
        }
        else
        {
            // Hit, with an escape
            double light_factor = 1.0 - (walk_result.escape_angle / escape_angle);

            light.color.r *= light_factor;
            light.color.g *= light_factor;
            light.color.b *= light_factor;

            return true;
        }
    }
    else
    {
        // No hit, leave light alone
        return true;
    }
}

void TerrainRenderer::estimateMinMaxHeight(double x1, double z1, double x2, double z2, double *ymin, double *ymax)
{
    double y1 = getHeight(x1, z1, true);
    double y2 = getHeight(x2, z2, true);

    // TODO Add quality factor
    // TODO Use all 4 corners
    // TODO Apply max slope
    // TODO Estimate displacement

    std::pair<double, double>minmax = std::minmax(y1, y2);
    *ymin = minmax.first;
    *ymax = minmax.second;
}
