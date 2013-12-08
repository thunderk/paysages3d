#include "AtmosphereRenderer.h"

#include <cmath>
#include "SoftwareRenderer.h"
#include "AtmosphereDefinition.h"
#include "AtmosphereModelBruneton.h"
#include "AtmosphereResult.h"
#include "LightComponent.h"
#include "LightStatus.h"
#include "Scenery.h"

/* Factor to convert software units to kilometers */
#define WORLD_SCALING 0.05
#define SUN_DISTANCE 149597870.0
#define SUN_DISTANCE_SCALED (SUN_DISTANCE / WORLD_SCALING)
#define SUN_RADIUS 6.955e5
#define SUN_RADIUS_SCALED (SUN_RADIUS / WORLD_SCALING)

static inline double _getDayFactor(double daytime)
{
    daytime = 1.0 - fabs(0.5 - daytime) / 0.5;
    return daytime < 0.45 ? 0.0 : sqrt((daytime - 0.45) / 0.55);
}

static inline void _applyWeatherEffects(AtmosphereDefinition* definition, AtmosphereResult* result)
{
    double distance = result->distance;
    double max_distance = 100.0 - 90.0 * definition->humidity;
    double distancefactor, dayfactor;

    if (distance > max_distance)
    {
        distance = max_distance;
    }
    distancefactor = (distance > max_distance ? max_distance : distance) / max_distance;
    /* TODO Get day lighting from model */
    dayfactor = _getDayFactor(definition->_daytime);

    /* Fog masking */
    if (definition->humidity > 0.3)
    {
        result->mask.r = result->mask.g = result->mask.b = (10.0 - 8.0 * definition->humidity) * dayfactor;
        result->mask.a = distancefactor * (definition->humidity - 0.3) / 0.7;
    }

    /* Scattering tweaking */
    if (definition->humidity < 0.15)
    {
        /* Limit scattering on ultra clear day */
        double force = (0.15 - definition->humidity) / 0.15;
        colorLimitPower(&result->inscattering, 100.0 - 90.0 * pow(force, 0.1));
    }
    else
    {
        /* Scattering boost */
        double force = 1.2 * (definition->humidity < 0.5 ? sqrt((definition->humidity - 0.15) / 0.35) : 1.0 - (definition->humidity - 0.5) / 0.5);
        result->inscattering.r *= 1.0 + force * distancefactor * (definition->humidity - 0.15) / 0.85;
        result->inscattering.g *= 1.0 + force * distancefactor * (definition->humidity - 0.15) / 0.85;
        result->inscattering.b *= 1.0 + force * distancefactor * (definition->humidity - 0.15) / 0.85;
    }

    /* Attenuation */
    result->attenuation.r *= 1.0 - 0.4 * distancefactor * definition->humidity;
    result->attenuation.g *= 1.0 - 0.4 * distancefactor * definition->humidity;
    result->attenuation.b *= 1.0 - 0.4 * distancefactor * definition->humidity;

    result->updateFinal();
}


BaseAtmosphereRenderer::BaseAtmosphereRenderer(SoftwareRenderer* renderer):
    renderer(renderer)
{

}

void BaseAtmosphereRenderer::getLightingStatus(LightStatus* status, Vector3, int)
{
    LightComponent light;

    light.color.r = 1.0;
    light.color.g = 1.0;
    light.color.b = 1.0;
    light.direction.x = -0.7;
    light.direction.y = -0.7;
    light.direction.z = 0.7;
    light.altered = 0;
    light.reflection = 0.0;
    status->pushComponent(light);
    light.color.r = 0.3;
    light.color.g = 0.31;
    light.color.b = 0.34;
    light.direction.x = 0.7;
    light.direction.y = -0.7;
    light.direction.z = -0.7;
    light.altered = 0;
    light.reflection = 0.0;
    status->pushComponent(light);
}

AtmosphereResult BaseAtmosphereRenderer::applyAerialPerspective(Vector3, Color base)
{
    AtmosphereResult result;
    result.base = result.final = base;
    result.inscattering = result.attenuation = COLOR_BLACK;
    return result;
}

AtmosphereResult BaseAtmosphereRenderer::getSkyColor(Vector3)
{
    AtmosphereResult result;
    result.base = result.final = COLOR_WHITE;
    result.inscattering = result.attenuation = COLOR_BLACK;
    return result;
}

Vector3 BaseAtmosphereRenderer::getSunDirection()
{
    AtmosphereDefinition* atmosphere = getDefinition();
    double sun_angle = (atmosphere->_daytime + 0.75) * M_PI * 2.0;
    return Vector3(cos(sun_angle), sin(sun_angle), 0.0);
}

AtmosphereDefinition* BaseAtmosphereRenderer::getDefinition()
{
    return renderer->getScenery()->getAtmosphere();
}

SoftwareBrunetonAtmosphereRenderer::SoftwareBrunetonAtmosphereRenderer(SoftwareRenderer* renderer):
    BaseAtmosphereRenderer(renderer)
{
    model = new AtmosphereModelBruneton(this);
}

SoftwareBrunetonAtmosphereRenderer::~SoftwareBrunetonAtmosphereRenderer()
{
    delete model;
}

void SoftwareBrunetonAtmosphereRenderer::getLightingStatus(LightStatus* status, Vector3 normal, int opaque)
{
    return model->fillLightingStatus(status, normal, opaque);
}

AtmosphereResult SoftwareBrunetonAtmosphereRenderer::applyAerialPerspective(Vector3 location, Color base)
{
    AtmosphereDefinition* definition = getDefinition();
    AtmosphereResult result;

    /* Get base perspective */
    switch (definition->model)
    {
    case AtmosphereDefinition::ATMOSPHERE_MODEL_BRUNETON:
        result = model->applyAerialPerspective(location, base);
        break;
    default:
        ;
    }

    /* Apply weather effects */
    _applyWeatherEffects(definition, &result);

    return result;
}

AtmosphereResult SoftwareBrunetonAtmosphereRenderer::getSkyColor(Vector3 direction)
{
    AtmosphereDefinition* definition;
    Vector3 sun_direction, sun_position, camera_location;
    Color base;

    definition = getDefinition();
    camera_location = renderer->getCameraLocation(renderer, VECTOR_ZERO);

    sun_direction = getSunDirection();
    direction = v3Normalize(direction);
    sun_position = v3Scale(sun_direction, SUN_DISTANCE_SCALED);

    /* Get sun shape */
    base = COLOR_BLACK;
    /*if (v3Dot(sun_direction, direction) >= 0)
    {
        double sun_radius = definition->sun_radius * SUN_RADIUS_SCALED * 5.0; // FIXME Why should we multiply by 5 ?
        Vector3 hit1, hit2;
        int hits = euclidRayIntersectSphere(camera_location, direction, sun_position, sun_radius, &hit1, &hit2);
        if (hits > 1)
        {
            double dist = v3Norm(v3Sub(hit2, hit1)) / sun_radius; // distance between intersection points (relative to radius)

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

    /* TODO Get stars */

    /* Get scattering */
    AtmosphereResult result;
    Vector3 location = v3Add(camera_location, v3Scale(direction, 6421.0));
    switch (definition->model)
    {
    case AtmosphereDefinition::ATMOSPHERE_MODEL_BRUNETON:
        result = model->getSkyColor(camera_location, direction, sun_position, base);
        break;
    default:
        result = BaseAtmosphereRenderer::applyAerialPerspective(location, result.base);
    }

    /* Apply weather effects */
    _applyWeatherEffects(definition, &result);

    return result;
}
