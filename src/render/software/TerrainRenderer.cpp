#include "TerrainRenderer.h"

#include <algorithm>
#include "SoftwareRenderer.h"
#include "Scenery.h"
#include "TerrainDefinition.h"
#include "TexturesRenderer.h"
#include "TexturesDefinition.h"
#include "LightComponent.h"
#include "TerrainRayWalker.h"
#include "RayCastingResult.h"

TerrainRenderer::TerrainRenderer(SoftwareRenderer *parent) : parent(parent) {
    walker_ray = new TerrainRayWalker(parent);
    walker_shadows = new TerrainRayWalker(parent);
    quad_normals = false;
}

TerrainRenderer::~TerrainRenderer() {
    delete walker_ray;
    delete walker_shadows;
}

void TerrainRenderer::update() {
    walker_ray->update();
    walker_shadows->update();
}

void TerrainRenderer::setQuality(bool quad_normals, double ray_precision, double shadow_precision) {
    this->quad_normals = quad_normals;

    walker_ray->setQuality(ray_precision);
    walker_shadows->setQuality(shadow_precision);
}

void TerrainRenderer::setQuality(double factor) {
    setQuality(factor > 0.6, factor, factor * factor);
}

double TerrainRenderer::getHeight(double x, double z, bool with_painting, bool water_offset) {
    return parent->getScenery()->getTerrain()->getInterpolatedHeight(x, z, true, with_painting, water_offset);
}

TerrainRenderer::TerrainResult TerrainRenderer::getResult(double x, double z, bool with_painting) {
    TerrainResult result;
    double offset = 0.001;

    /* Normal */
    Vector3 center, north, east, south, west;

    center.x = x;
    center.z = z;
    center.y = getHeight(center.x, center.z, with_painting);

    east.x = x + offset;
    east.z = z;
    east.y = getHeight(east.x, east.z, with_painting);

    south.x = x;
    south.z = z + offset;
    south.y = getHeight(south.x, south.z, with_painting);

    if (parent->render_quality > 6) {
        west.x = x - offset;
        west.z = z;
        west.y = getHeight(west.x, west.z, with_painting);

        north.x = x;
        north.z = z - offset;
        north.y = getHeight(north.x, north.z, with_painting);

        result.normal = center.getNormal5(south, north, east, west);
    } else {
        result.normal = center.getNormal3(south, east);
    }

    /* Location */
    result.location = center;

    return result;
}

Vector3 TerrainRenderer::getDisplaced(double x, double z, bool with_painting) {
    auto terrain = getResult(x, z, with_painting);
    auto textures = parent->getScenery()->getTextures();
    return parent->getTexturesRenderer()->displaceTerrain(textures, terrain.location, terrain.normal);
}

static inline pair<vector<double>, vector<Vector3>> _getTexturesInfo(TerrainRenderer *terrain_renderer,
                                                                     TexturesRenderer *textures_renderer, double x,
                                                                     double z,
                                                                     TexturesDefinition *textures_definition) {
    auto terrain = terrain_renderer->getResult(x, z, true);
    auto presence = textures_renderer->getLayersPresence(textures_definition, terrain.location, terrain.normal);
    auto displaced =
        textures_renderer->getLayersDisplacement(textures_definition, terrain.location, terrain.normal, presence);
    return pair<vector<double>, vector<Vector3>>(presence, displaced);
}

Color TerrainRenderer::getFinalColor(double x, double z, double precision) {
    auto textures_renderer = parent->getTexturesRenderer();
    auto textures_definition = parent->getScenery()->getTextures();

    if (textures_definition->getLayerCount() == 0) {
        return COLOR_BLACK;
    } else {
        auto current = _getTexturesInfo(this, textures_renderer, x, z, textures_definition);
        auto top_location = current.second.back();

        vector<Vector3> normal;
        int i = 0;
        // TODO Use getNormal5 on high-quality renders
        double offset = 0.0001;
        auto east = _getTexturesInfo(this, textures_renderer, x + offset, z, textures_definition);
        auto south = _getTexturesInfo(this, textures_renderer, x, z + offset, textures_definition);
        for (auto layer_presence : current.first) {
            if (layer_presence > 0.0) {
                normal.push_back(current.second[i].getNormal3(south.second[i], east.second[i]));
            } else {
                normal.push_back(VECTOR_ZERO);
            }
            i++;
        }

        auto color = textures_renderer->getFinalComposition(textures_definition, parent->getLightingManager(),
                                                            current.first, current.second, normal, precision,
                                                            parent->getCameraLocation(top_location));

        return parent->applyMediumTraversal(top_location, color);
    }
}

RayCastingResult TerrainRenderer::castRay(const Vector3 &start, const Vector3 &direction) {
    RayCastingResult result;
    TerrainRayWalker::TerrainHitResult walk_result;
    if (walker_ray->startWalking(start, direction.normalize(), 0.0, walk_result)) {
        result.hit = true;
        result.hit_location = walk_result.hit_location;
        result.hit_color = getFinalColor(walk_result.hit_location.x, walk_result.hit_location.z,
                                         parent->getPrecision(walk_result.hit_location));
    } else {
        result.hit = false;
    }
    return result;
}

bool TerrainRenderer::applyLightFilter(LightComponent &light, const Vector3 &at) {
    TerrainDefinition *definition = parent->getScenery()->getTerrain();
    TerrainRayWalker::TerrainHitResult walk_result;

    // If location is above terrain, don't bother
    if (at.y > definition->getHeightInfo().max_height) {
        return true;
    }

    // Handle sun below horizon
    Vector3 direction_to_light = light.direction.scale(-1.0);
    if (direction_to_light.y < -0.05) {
        light.color = COLOR_BLACK;
        return false;
    } else if (direction_to_light.y < 0.0000) {
        light.color.r *= (0.05 + direction_to_light.y) / 0.05;
        light.color.g *= (0.05 + direction_to_light.y) / 0.05;
        light.color.b *= (0.05 + direction_to_light.y) / 0.05;
    }

    // Walk to find an intersection
    double escape_angle = definition->shadow_smoothing;
    // TODO max length should depend on the sun light angle and altitude range
    if (walker_shadows->startWalking(at, direction_to_light, escape_angle, walk_result)) {
        if (walk_result.escape_angle == 0.0) {
            // Hit, with no escape, cancelling the light
            light.color = COLOR_BLACK;
            return false;
        } else {
            // Hit, with an escape
            double light_factor = 1.0 - (walk_result.escape_angle / escape_angle);

            light.color.r *= light_factor;
            light.color.g *= light_factor;
            light.color.b *= light_factor;

            return true;
        }
    } else {
        // No hit, leave light alone
        return true;
    }
}

void TerrainRenderer::estimateMinMaxHeight(double x1, double z1, double x2, double z2, double *ymin, double *ymax) {
    double y1 = getHeight(x1, z1, true);
    double y2 = getHeight(x2, z2, true);

    // TODO Add quality factor
    // TODO Use all 4 corners
    // TODO Apply max slope
    // TODO Estimate displacement

    pair<double, double> limits = minmax(y1, y2);
    *ymin = limits.first;
    *ymax = limits.second;
}
