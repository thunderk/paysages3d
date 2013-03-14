#include "private.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "../tools.h"
#include "../renderer.h"
#include "../system.h"

/******************** Fake ********************/
static AtmosphereResult _fakeApplyAerialPerspective(Renderer* renderer, Vector3 location, Color base)
{
    AtmosphereResult result;
    UNUSED(renderer);
    UNUSED(location);
    result.base = result.final = base;
    result.inscattering = result.attenuation = COLOR_BLACK;
    return result;
}

static AtmosphereResult _fakeGetSkyColor(Renderer* renderer, Vector3 direction)
{
    AtmosphereResult result;
    UNUSED(renderer);
    UNUSED(direction);
    result.base = result.final = COLOR_WHITE;
    result.inscattering = result.attenuation = COLOR_BLACK;
    return result;
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
static inline void _applyWeatherEffects(AtmosphereDefinition* definition, AtmosphereResult* result)
{
    double distance = result->distance;

    if (distance > 100.0)
    {
        distance = 100.0;
    }
    distance /= 100.0;

    result->inscattering.r += distance * 0.2 * definition->humidity;
    result->inscattering.g += distance * 0.2 * definition->humidity;
    result->inscattering.b += distance * 0.2 * definition->humidity;

    result->attenuation.r *= 1.0 - distance * definition->humidity;
    result->attenuation.g *= 1.0 - distance * definition->humidity;
    result->attenuation.b *= 1.0 - distance * definition->humidity;

    atmosphereUpdateResult(result);
}

static AtmosphereResult _realApplyAerialPerspective(Renderer* renderer, Vector3 location, Color base)
{
    AtmosphereDefinition* definition = renderer->atmosphere->definition;
    AtmosphereResult result;

    /* Get base perspective */
    switch (definition->model)
    {
    case ATMOSPHERE_MODEL_BRUNETON:
        result = brunetonApplyAerialPerspective(renderer, location, base);
        break;
    default:
        ;
    }

    /* Apply weather effects */
    /*_applyWeatherEffects(definition, &result);*/

    return result;
}

static AtmosphereResult _realGetSkyColor(Renderer* renderer, Vector3 direction)
{
    AtmosphereDefinition* definition;
    Vector3 sun_direction, sun_position, camera_location;
    Color base;

    definition = renderer->atmosphere->definition;
    camera_location = renderer->getCameraLocation(renderer, VECTOR_ZERO);

    sun_direction = renderer->atmosphere->getSunDirection(renderer);
    direction = v3Normalize(direction);
    sun_position = v3Scale(sun_direction, SUN_DISTANCE_SCALED);

    /* Get sun shape */
    base = COLOR_BLACK;
    if (v3Dot(sun_direction, direction) >= 0)
    {
        double sun_radius = definition->sun_radius * SUN_RADIUS_SCALED * 5.0; /* FIXME Why should we multiply by 5 ? */
        Vector3 hit1, hit2;
        int hits = euclidRayIntersectSphere(camera_location, direction, sun_position, sun_radius, &hit1, &hit2);
        if (hits > 1)
        {
            double dist = v3Norm(v3Sub(hit2, hit1)) / sun_radius; /* distance between intersection points (relative to radius) */

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
    }

    /* TODO Get stars */

    /* Get scattering */
    AtmosphereResult result;
    Vector3 location = v3Add(camera_location, v3Scale(direction, 6421.0));
    switch (definition->model)
    {
    case ATMOSPHERE_MODEL_BRUNETON:
        result = brunetonGetSkyColor(renderer, camera_location, direction, sun_position, base);
        break;
    default:
        result = _fakeApplyAerialPerspective(renderer, location, result.base);
    }

    /* Apply weather effects */
    /*_applyWeatherEffects(definition, &result);*/

    return result;
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

void atmosphereUpdateResult(AtmosphereResult* result)
{
    result->final.r = result->base.r * result->attenuation.r + result->inscattering.r;
    result->final.g = result->base.g * result->attenuation.g + result->inscattering.g;
    result->final.b = result->base.b * result->attenuation.b + result->inscattering.b;
    result->final.a = 1.0;
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
            renderer->atmosphere->getLightingStatus = _fakeGetLightingStatus;
    }
}

StandardRenderer AtmosphereRendererClass = {
    (FuncObjectCreate)_createRenderer,
    (FuncObjectDelete)_deleteRenderer,
    (FuncObjectBind)_bindRenderer
};
