#include "WaterRenderer.h"

#include <cmath>
#include "SoftwareRenderer.h"
#include "TerrainRenderer.h"
#include "WaterDefinition.h"
#include "NoiseGenerator.h"
#include "LightComponent.h"
#include "Scenery.h"
#include "SurfaceMaterial.h"
#include "NoiseFunctionSimplex.h"
#include "FloatNode.h"
#include "ColorNode.h"
#include "RayCastingResult.h"

WaterRenderer::WaterRenderer(SoftwareRenderer *parent) : parent(parent) {
    noise = new NoiseFunctionSimplex();
}

WaterRenderer::~WaterRenderer() {
    delete noise;
}

void WaterRenderer::update() {
    WaterDefinition *definition = parent->getScenery()->getWater();
    noise->setState(*definition->noise_state);
    noise->setScaling(definition->scaling * 0.3, definition->waves_height * 0.05);
    noise->setStep(0.3);
}

static inline double _getHeight(WaterDefinition *definition, FractalNoise *noise, double x, double z) {
    return noise->get2d(0.00001, x + definition->propXOffset()->getValue(), z + definition->propZOffset()->getValue());
}

static inline Vector3 _getNormal(WaterDefinition *definition, FractalNoise *noise, Vector3 base, double detail) {
    Vector3 back, right;
    double x, z;

    x = base.x;
    z = base.z;

    back.x = x;
    back.y = _getHeight(definition, noise, x, z + detail);
    back.z = z + detail;
    back = back.sub(base);

    right.x = x + detail;
    right.y = _getHeight(definition, noise, x + detail, z);
    right.z = z;
    right = right.sub(base);

    return back.crossProduct(right).normalize();
}

static inline Vector3 _reflectRay(Vector3 incoming, Vector3 normal) {
    double c;

    c = normal.dotProduct(incoming.scale(-1.0));
    return incoming.add(normal.scale(2.0 * c));
}

static inline Vector3 _refractRay(Vector3 incoming, Vector3 normal) {
    double c1, c2, f;

    f = 1.0 / 1.33;
    c1 = normal.dotProduct(incoming.scale(-1.0));
    c2 = sqrt(1.0 - pow(f, 2.0) * (1.0 - pow(c1, 2.0)));
    if (c1 >= 0.0) {
        return incoming.scale(f).add(normal.scale(f * c1 - c2));
    } else {
        return incoming.scale(f).add(normal.scale(c2 - f * c1));
    }
}

static inline Color _getFoamMask(SoftwareRenderer *renderer, WaterDefinition *definition, FractalNoise *noise,
                                 Vector3 location, Vector3 normal, double detail) {
    Color result;
    double foam_factor, normal_diff, location_offset;

    location_offset = 2.0 * detail;

    foam_factor = 0.0;
    location.x += location_offset;
    normal_diff = 1.0 - normal.dotProduct(_getNormal(definition, noise, location, detail));
    if (normal_diff > foam_factor) {
        foam_factor = normal_diff;
    }
    location.x -= location_offset * 2.0;
    normal_diff = 1.0 - normal.dotProduct(_getNormal(definition, noise, location, detail));
    if (normal_diff > foam_factor) {
        foam_factor = normal_diff;
    }
    location.x += location_offset;
    location.z -= location_offset;
    normal_diff = 1.0 - normal.dotProduct(_getNormal(definition, noise, location, detail));
    if (normal_diff > foam_factor) {
        foam_factor = normal_diff;
    }
    location.z += location_offset * 2.0;
    normal_diff = 1.0 - normal.dotProduct(_getNormal(definition, noise, location, detail));
    if (normal_diff > foam_factor) {
        foam_factor = normal_diff;
    }

    foam_factor *= 10.0;
    if (foam_factor > 1.0) {
        foam_factor = 1.0;
    }

    if (foam_factor <= 1.0 - definition->foam_coverage) {
        return COLOR_TRANSPARENT;
    }
    foam_factor = (foam_factor - (1.0 - definition->foam_coverage)) * definition->foam_coverage;

    /* TODO Re-use base lighting status */
    result = renderer->applyLightingToSurface(location, normal, *definition->foam_material);
    result.r *= 2.0;
    result.g *= 2.0;
    result.b *= 2.0;

    /* TODO This should be configurable */
    if (foam_factor > 0.2) {
        result.a = 0.8;
    } else {
        result.a = 0.8 * (foam_factor / 0.2);
    }

    return result;
}

HeightInfo WaterRenderer::getHeightInfo() {
    HeightInfo info;
    double noise_minvalue, noise_maxvalue;

    info.base_height = 0.0;
    // TODO
    // noise->getRange(&noise_minvalue, &noise_maxvalue);
    noise_minvalue = noise_maxvalue = 0.0;
    info.min_height = info.base_height + noise_minvalue;
    info.max_height = info.base_height + noise_maxvalue;

    return info;
}

double WaterRenderer::getHeight(double x, double z) {
    return _getHeight(parent->getScenery()->getWater(), noise, x, z);
}

WaterRenderer::WaterResult WaterRenderer::getResult(double x, double z) {
    WaterDefinition *definition = parent->getScenery()->getWater();
    WaterResult result;
    RayCastingResult refracted;
    Vector3 location, normal, look_direction;
    Color color, foam;
    double detail, depth;
    double reflection = definition->propReflection()->getValue();

    location.x = x;
    location.y = _getHeight(definition, noise, x, z);
    location.z = z;
    result.location = location;

    detail = parent->getPrecision(location) * 0.1;
    if (detail < 0.00001) {
        detail = 0.00001;
    }

    normal = _getNormal(definition, noise, location, detail);
    look_direction = location.sub(parent->getCameraLocation(location)).normalize();

    /* Reflection */
    if (reflection == 0.0) {
        result.reflected = COLOR_BLACK;
    } else {
        result.reflected = parent->rayWalking(location, _reflectRay(look_direction, normal), 1, 0, 1, 1).hit_color;
    }

    /* Transparency/refraction */
    if (definition->transparency == 0.0) {
        result.refracted = COLOR_BLACK;
    } else {
        Color depth_color = definition->propDepthColor()->getValue();
        refracted = parent->rayWalking(location, _refractRay(look_direction, normal), 1, 0, 1, 1);
        depth = location.sub(refracted.hit_location).getNorm();
        depth_color.limitPower(refracted.hit_color.getPower());
        if (depth > definition->transparency_depth) {
            result.refracted = depth_color;
        } else {
            depth /= definition->transparency_depth;
            result.refracted.r = refracted.hit_color.r * (1.0 - depth) + depth_color.r * depth;
            result.refracted.g = refracted.hit_color.g * (1.0 - depth) + depth_color.g * depth;
            result.refracted.b = refracted.hit_color.b * (1.0 - depth) + depth_color.b * depth;
            result.refracted.a = 1.0;
        }
    }

    /* Lighting from environment */
    color = parent->applyLightingToSurface(location, normal, *definition->material);

    color.r += result.reflected.r * reflection + result.refracted.r * definition->transparency;
    color.g += result.reflected.g * reflection + result.refracted.g * definition->transparency;
    color.b += result.reflected.b * reflection + result.refracted.b * definition->transparency;

    /* Merge with foam */
    foam = _getFoamMask(parent, definition, noise, location, normal, detail);
    color.mask(foam);

    /* Bring color to the camera */
    color = parent->applyMediumTraversal(location, color);

    result.base = *definition->material->base;
    result.final = color;

    return result;
}

bool WaterRenderer::applyLightFilter(LightComponent &light, const Vector3 &at) {
    WaterDefinition *definition = parent->getScenery()->getWater();
    double factor;

    if (at.y < 0.0) {
        if (light.direction.y <= -0.00001) {
            factor = -at.y / (-light.direction.y * definition->lighting_depth);
            if (factor > 1.0) {
                factor = 1.0;
            }
            factor = 1.0 - factor;

            light.color.r *= factor;
            light.color.g *= factor;
            light.color.b *= factor;
            light.reflection *= factor;

            return factor > 0.0;
        } else {
            light.color = COLOR_BLACK;
            return false;
        }
    } else {
        return true;
    }
}
