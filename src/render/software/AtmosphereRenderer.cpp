#include "AtmosphereRenderer.h"

#include <cmath>
#include "SoftwareRenderer.h"
#include "AtmosphereDefinition.h"
#include "AtmosphereModelBruneton.h"
#include "AtmosphereResult.h"
#include "GodRaysSampler.h"
#include "LightComponent.h"
#include "LightStatus.h"
#include "Scenery.h"
#include "NightSky.h"
#include "FloatNode.h"

/* Factor to convert software units to kilometers */
#define WORLD_SCALING 0.05
#define SUN_DISTANCE 149597870.0
#define SUN_DISTANCE_SCALED (SUN_DISTANCE / WORLD_SCALING)
#define SUN_RADIUS 6.955e5
#define SUN_RADIUS_SCALED (SUN_RADIUS / WORLD_SCALING)

static inline double _getDayFactor(double daytime) {
    daytime = 1.0 - fabs(0.5 - daytime) / 0.5;
    return daytime < 0.45 ? 0.0 : sqrt((daytime - 0.45) / 0.55);
}

static inline void _applyWeatherEffects(AtmosphereDefinition *definition, AtmosphereResult *result) {
    if (definition->model == AtmosphereDefinition::ATMOSPHERE_MODEL_DISABLED) {
        result->updateFinal();
        return;
    }

    double humidity = definition->propHumidity()->getValue();
    double distance = result->distance;
    double max_distance = 100.0 - 90.0 * humidity;
    double distancefactor, dayfactor;

    if (distance > max_distance) {
        distance = max_distance;
    }
    distancefactor = (distance > max_distance ? max_distance : distance) / max_distance;
    /* TODO Get day lighting from model */
    dayfactor = _getDayFactor(definition->propDayTime()->getValue());

    /* Fog masking */
    if (humidity > 0.3) {
        result->mask.r = result->mask.g = result->mask.b = (10.0 - 8.0 * humidity) * dayfactor;
        result->mask.a = distancefactor * (humidity - 0.3) / 0.7;
    }

    /* Scattering tweaking */
    if (humidity < 0.15) {
        /* Limit scattering on ultra clear day */
        double force = (0.15 - humidity) / 0.15;
        result->inscattering.limitPower(100.0 - 90.0 * pow(force, 0.1));
    } else {
        /* Scattering boost */
        double force = 1.2 * (humidity < 0.5 ? sqrt((humidity - 0.15) / 0.35) : 1.0 - (humidity - 0.5) / 0.5);
        result->inscattering.r *= 1.0 + force * distancefactor * (humidity - 0.15) / 0.85;
        result->inscattering.g *= 1.0 + force * distancefactor * (humidity - 0.15) / 0.85;
        result->inscattering.b *= 1.0 + force * distancefactor * (humidity - 0.15) / 0.85;
    }

    /* Attenuation */
    result->attenuation.r *= 1.0 - 0.4 * distancefactor * humidity;
    result->attenuation.g *= 1.0 - 0.4 * distancefactor * humidity;
    result->attenuation.b *= 1.0 - 0.4 * distancefactor * humidity;

    result->updateFinal();
}

BaseAtmosphereRenderer::BaseAtmosphereRenderer(SoftwareRenderer *renderer) : parent(renderer) {
}

AtmosphereResult BaseAtmosphereRenderer::applyAerialPerspective(const Vector3 &, const Color &base) {
    AtmosphereResult result;
    result.base = result.final = base;
    result.inscattering = result.attenuation = COLOR_BLACK;
    return result;
}

AtmosphereResult BaseAtmosphereRenderer::getSkyColor(const Vector3 &) {
    AtmosphereResult result;
    result.base = result.final = COLOR_WHITE;
    result.inscattering = result.attenuation = COLOR_BLACK;
    return result;
}

Vector3 BaseAtmosphereRenderer::getSunDirection(bool) const {
    AtmosphereDefinition *atmosphere = getDefinition();
    double sun_angle = (atmosphere->propDayTime()->getValue() + 0.75) * M_PI * 2.0;
    return Vector3(cos(sun_angle), sin(sun_angle), 0.0);
}

bool BaseAtmosphereRenderer::getLightsAt(vector<LightComponent> &, const Vector3 &) const {
    return false;
}

AtmosphereDefinition *BaseAtmosphereRenderer::getDefinition() const {
    return parent->getScenery()->getAtmosphere();
}

SoftwareBrunetonAtmosphereRenderer::SoftwareBrunetonAtmosphereRenderer(SoftwareRenderer *renderer)
    : BaseAtmosphereRenderer(renderer) {
    model = new AtmosphereModelBruneton(parent);
}

SoftwareBrunetonAtmosphereRenderer::~SoftwareBrunetonAtmosphereRenderer() {
    delete model;
}

AtmosphereResult SoftwareBrunetonAtmosphereRenderer::applyAerialPerspective(const Vector3 &location,
                                                                            const Color &base) {
    AtmosphereDefinition *definition = getDefinition();
    AtmosphereResult result;

    // Get base perspective
    switch (definition->model) {
    case AtmosphereDefinition::ATMOSPHERE_MODEL_BRUNETON:
        result = model->applyAerialPerspective(location, base);
        break;
    default:
        ;
    }

    // Apply god rays ponderation
    result.inscattering = parent->getGodRaysSampler()->apply(COLOR_BLACK, result.inscattering, location);

    // Apply weather effects
    _applyWeatherEffects(definition, &result);

    return result;
}

AtmosphereResult SoftwareBrunetonAtmosphereRenderer::getSkyColor(const Vector3 &direction) {
    AtmosphereDefinition *definition;
    Vector3 sun_direction, sun_position, camera_location;
    Color base;

    definition = getDefinition();
    camera_location = parent->getCameraLocation(VECTOR_ZERO);

    sun_direction = getSunDirection();
    Vector3 direction_norm = direction.normalize();
    sun_position = sun_direction.scale(SUN_DISTANCE_SCALED);

    base = COLOR_BLACK;

    // Get night sky
    base = base.add(parent->getNightSky()->getColor(camera_location.y, direction_norm));

    // Get sun shape
    /*if (v3Dot(sun_direction, direction) >= 0)
    {
        double sun_radius = definition->sun_radius * SUN_RADIUS_SCALED * 5.0; // FIXME Why should we multiply by 5 ?
        Vector3 hit1, hit2;
        int hits = euclidRayIntersectSphere(camera_location, direction, sun_position, sun_radius, &hit1, &hit2);
        if (hits > 1)
        {
            double dist = v3Norm(v3Sub(hit2, hit1)) / sun_radius; // distance between intersection points (relative to
    radius)

            Color sun_color = definition->sun_color;
            sun_color.r *= 100.0;
            sun_color.g *= 100.0;
            sun_color.b *= 100.0;

            if (dist <= 0.05)
            {
                sun_color.r *= 1.0 - dist / 0.05;
                sun_color.g *= 1.0 - dist / 0.05;
                sun_color.b *= 1.0 - dist / 0.05;
            }
            base = sun_color;
        }
    }*/

    // Get scattering
    AtmosphereResult result;
    Vector3 location = camera_location.add(direction_norm.scale(6421.0));
    switch (definition->model) {
    case AtmosphereDefinition::ATMOSPHERE_MODEL_BRUNETON:
        result = model->getSkyColor(camera_location, direction_norm, sun_position, base);
        break;
    default:
        result = BaseAtmosphereRenderer::applyAerialPerspective(location, result.base);
    }

    // Apply god rays ponderation
    result.inscattering = parent->getGodRaysSampler()->apply(COLOR_BLACK, result.inscattering, location);

    // Apply weather effects
    _applyWeatherEffects(definition, &result);

    return result;
}

bool SoftwareBrunetonAtmosphereRenderer::getLightsAt(vector<LightComponent> &result, const Vector3 &location) const {
    bool changed = false;
    changed |= model->getLightsAt(result, location);
    changed |= parent->getNightSky()->getLightsAt(result, location);
    return changed;
}
