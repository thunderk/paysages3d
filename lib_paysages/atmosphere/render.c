#include "private.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "../tools.h"
#include "../renderer.h"
#include "../system.h"

/******************** Fake ********************/
static Color _fakeApplyAerialPerspective(Renderer* renderer, Vector3 location, Color base)
{
    UNUSED(renderer);
    UNUSED(location);
    return base;
}

static Color _fakeGetSkyColor(Renderer* renderer, Vector3 direction)
{
    UNUSED(renderer);
    UNUSED(direction);
    return COLOR_WHITE;
}

static void _fakeGetLightingStatus(Renderer* renderer, LightStatus* status, Vector3 normal, int opaque)
{
    LightDefinition light;

    UNUSED(renderer);
    UNUSED(normal);
    UNUSED(opaque);

    light.color.r = 0.8;
    light.color.g = 0.8;
    light.color.b = 0.8;
    light.direction.x = -0.7;
    light.direction.y = -0.7;
    light.direction.z = 0.7;
    light.altered = 0;
    light.reflection = 0.0;
    lightingPushLight(status, &light);
    light.color.r = 0.3;
    light.color.g = 0.31;
    light.color.b = 0.34;
    light.direction.x = 0.7;
    light.direction.y = -0.7;
    light.direction.z = -0.7;
    light.altered = 0;
    light.reflection = 0.0;
    lightingPushLight(status, &light);
}

/******************** Real ********************/
static inline Color _applyWeatherEffects(AtmosphereDefinition* definition, Color base, Color scattered, double distance)
{
    double max_power = colorGetPower(&scattered);

    UNUSED(base);

    if (distance > 100.0)
    {
        distance = 100.0;
    }

    scattered.r += distance * 0.02 * definition->humidity;
    scattered.g += distance * 0.02 * definition->humidity;
    scattered.b += distance * 0.02 * definition->humidity;

    colorLimitPower(&scattered, max_power - max_power * 0.4 * definition->humidity);

    if (definition->humidity > 0.3)
    {
        double humidity = (definition->humidity - 0.3) / 0.7;
        scattered.r += distance * 0.1 * humidity * humidity;
        scattered.g += distance * 0.1 * humidity * humidity;
        scattered.b += distance * 0.1 * humidity * humidity;

        colorLimitPower(&scattered, 10.0 - humidity * 4.0);
        /*scattered.r *= 1.0 - humidity * 0.8;
        scattered.g *= 1.0 - humidity * 0.8;
        scattered.b *= 1.0 - humidity * 0.8;*/
    }

    return scattered;
}

static Color _realApplyAerialPerspective(Renderer* renderer, Vector3 location, Color base)
{
    AtmosphereDefinition* definition = renderer->atmosphere->definition;
    Color changed;

    /* Get base perspective */
    switch (definition->model)
    {
    case ATMOSPHERE_MODEL_BRUNETON:
        changed = brunetonApplyAerialPerspective(renderer, location, base);
        break;
    case ATMOSPHERE_MODEL_PREETHAM:
        changed = basicApplyAerialPerspective(renderer, location, base);
        break;
    default:
        ;
    }

    /* Apply weather effects */
    changed = _applyWeatherEffects(definition, base, changed, v3Norm(v3Sub(location, renderer->getCameraLocation(renderer, location))));

    return changed;
}

static Color _realGetSkyColor(Renderer* renderer, Vector3 direction)
{
    AtmosphereDefinition* definition;
    Vector3 sun_direction, sun_position, camera_location;
    Color sky_color, sun_color;

    definition = renderer->atmosphere->definition;
    camera_location = renderer->getCameraLocation(renderer, VECTOR_ZERO);

    sun_direction = renderer->atmosphere->getSunDirection(renderer);
    direction = v3Normalize(direction);
    sun_position = v3Scale(sun_direction, SUN_DISTANCE_SCALED);

    /* Get base scattering*/
    switch (definition->model)
    {
    case ATMOSPHERE_MODEL_BRUNETON:
        sky_color = brunetonGetSkyColor(renderer, camera_location, direction, sun_position);
        break;
    case ATMOSPHERE_MODEL_PREETHAM:
        sky_color = preethamGetSkyColor(definition, camera_location, direction, sun_position);
        break;
    default:
        sky_color = COLOR_BLUE;
    }

    /* Get sun shape */
    if (v3Dot(sun_direction, direction) >= 0)
    {
        double sun_radius = definition->sun_radius * SUN_RADIUS_SCALED * 5.0; /* FIXME Why should we multiply by 5 ? */
        Vector3 hit1, hit2;
        int hits = euclidRayIntersectSphere(camera_location, direction, sun_position, sun_radius, &hit1, &hit2);
        if (hits > 1)
        {
            double dist = v3Norm(v3Sub(hit2, hit1)) / sun_radius; /* distance between intersection points (relative to radius) */

            sun_color = definition->sun_color;
            sun_color.r *= 100.0;
            sun_color.g *= 100.0;
            sun_color.b *= 100.0;

            if (dist > 0.05)
            {
                return sun_color;
            }
            else
            {
                sun_color.a = 1.0 - dist / 0.05;
                colorMask(&sky_color, &sun_color);
            }
        }
    }

    /* TODO Apply weather effects */
    sky_color = _applyWeatherEffects(definition, COLOR_BLACK, sky_color, SPHERE_SIZE);

    return sky_color;
}

static Vector3 _realGetSunDirection(Renderer* renderer)
{
    Vector3 result;
    double sun_angle = (renderer->atmosphere->definition->_daytime + 0.75) * M_PI * 2.0;
    result.x = cos(sun_angle);
    result.y = sin(sun_angle);
    result.z = 0.0;
    return result;
}

/******************** Renderer ********************/
static AtmosphereRenderer* _createRenderer()
{
    AtmosphereRenderer* result;

    result = malloc(sizeof(AtmosphereRenderer));
    result->definition = AtmosphereDefinitionClass.create();

    result->getLightingStatus = _fakeGetLightingStatus;
    result->getSunDirection = _realGetSunDirection;
    result->applyAerialPerspective = _fakeApplyAerialPerspective;
    result->getSkyColor = _fakeGetSkyColor;

    return result;
}

static void _deleteRenderer(AtmosphereRenderer* renderer)
{
    AtmosphereDefinitionClass.destroy(renderer->definition);
    free(renderer);
}

static void _bindRenderer(Renderer* renderer, AtmosphereDefinition* definition)
{
    AtmosphereDefinitionClass.copy(definition, renderer->atmosphere->definition);

    renderer->atmosphere->getSkyColor = _realGetSkyColor;
    renderer->atmosphere->applyAerialPerspective = _realApplyAerialPerspective;

    switch (definition->model)
    {
        case ATMOSPHERE_MODEL_BRUNETON:
            renderer->atmosphere->getLightingStatus = brunetonGetLightingStatus;
            break;
        default:
            renderer->atmosphere->getLightingStatus = basicGetLightingStatus;
    }
}

StandardRenderer AtmosphereRendererClass = {
    (FuncObjectCreate)_createRenderer,
    (FuncObjectDelete)_deleteRenderer,
    (FuncObjectBind)_bindRenderer
};
