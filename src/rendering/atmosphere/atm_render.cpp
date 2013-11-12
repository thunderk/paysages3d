#include "private.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "../tools.h"
#include "../renderer.h"

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

    light.color.r = 1.0;
    light.color.g = 1.0;
    light.color.b = 1.0;
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

static Vector3 _realGetSunDirection(Renderer* renderer)
{
    Vector3 result;
    double sun_angle = (renderer->atmosphere->definition->_daytime + 0.75) * M_PI * 2.0;
    result.x = cos(sun_angle);
    result.y = sin(sun_angle);
    result.z = 0.0;
    return result;
}

void atmosphereInitResult(AtmosphereResult* result)
{
    result->base = COLOR_BLACK;
    result->inscattering = COLOR_BLACK;
    result->attenuation = COLOR_WHITE;
    result->mask = COLOR_TRANSPARENT;
    result->distance = 0.0;
    result->final = COLOR_BLACK;
}

void atmosphereUpdateResult(AtmosphereResult* result)
{
    result->final.r = result->base.r * result->attenuation.r + result->inscattering.r;
    result->final.g = result->base.g * result->attenuation.g + result->inscattering.g;
    result->final.b = result->base.b * result->attenuation.b + result->inscattering.b;
    result->final.a = 1.0;

    colorMask(&result->final, &result->mask);
}

/******************** Renderer ********************/
static AtmosphereRenderer* _createRenderer()
{
    AtmosphereRenderer* result;

    result = new AtmosphereRenderer;
    result->definition = (AtmosphereDefinition*)AtmosphereDefinitionClass.create();

    result->getLightingStatus = _fakeGetLightingStatus;
    result->getSunDirection = _realGetSunDirection;
    result->applyAerialPerspective = _fakeApplyAerialPerspective;
    result->getSkyColor = _fakeGetSkyColor;

    return result;
}

static void _deleteRenderer(AtmosphereRenderer* renderer)
{
    AtmosphereDefinitionClass.destroy(renderer->definition);
    delete renderer;
}

static void _bindRenderer(Renderer* renderer, AtmosphereDefinition* definition)
{
    AtmosphereDefinitionClass.copy(definition, renderer->atmosphere->definition);
}

StandardRenderer AtmosphereRendererClass = {
    (FuncObjectCreate)_createRenderer,
    (FuncObjectDelete)_deleteRenderer,
    (FuncObjectBind)_bindRenderer
};
