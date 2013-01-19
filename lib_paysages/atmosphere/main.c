#include "private.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "../tools.h"
#include "../renderer.h"
#include "../system.h"

#define MAX_SKYDOME_LIGHTS 100

typedef struct
{
    Mutex* lock;
    int nblights;
    LightDefinition lights[MAX_SKYDOME_LIGHTS];
} AtmosphereRendererCache;

static int _inited = 0;

/******************** Definition ********************/
static void _validateDefinition(AtmosphereDefinition* definition)
{
    if (definition->hour < 0)
    {
        definition->hour = 0;
    }
    if (definition->hour > 23)
    {
        definition->hour = 23;
    }
    if (definition->minute < 0)
    {
        definition->minute = 0;
    }
    if (definition->minute > 59)
    {
        definition->minute = 59;
    }

    definition->_daytime = (double)definition->hour / 24.0 + (double)definition->minute / 1440.0;
}

static AtmosphereDefinition* _createDefinition()
{
    AtmosphereDefinition* result;

    /* TODO Find a better place for this */
    if (!_inited)
    {
        _inited = 1;
        brunetonInit();
    }

    result = malloc(sizeof(AtmosphereDefinition));

    atmosphereAutoPreset(result, ATMOSPHERE_PRESET_CLEAR_DAY);

    return result;
}

static void _deleteDefinition(AtmosphereDefinition* definition)
{
    free(definition);
}

static void _copyDefinition(AtmosphereDefinition* source, AtmosphereDefinition* destination)
{
    destination->model = source->model;
    destination->hour = source->hour;
    destination->minute = source->minute;
    destination->sun_color = source->sun_color;
    destination->sun_radius = source->sun_radius;
    destination->dome_lighting = source->dome_lighting;
    destination->humidity = source->humidity;

    _validateDefinition(destination);
}

static void _saveDefinition(PackStream* stream, AtmosphereDefinition* definition)
{
    packWriteInt(stream, (int*)&definition->model);
    packWriteInt(stream, &definition->hour);
    packWriteInt(stream, &definition->minute);
    colorSave(stream, &definition->sun_color);
    packWriteDouble(stream, &definition->sun_radius);
    packWriteDouble(stream, &definition->dome_lighting);
    packWriteDouble(stream, &definition->humidity);
}

static void _loadDefinition(PackStream* stream, AtmosphereDefinition* definition)
{
    packReadInt(stream, (int*)&definition->model);
    packReadInt(stream, &definition->hour);
    packReadInt(stream, &definition->minute);
    colorLoad(stream, &definition->sun_color);
    packReadDouble(stream, &definition->sun_radius);
    packReadDouble(stream, &definition->dome_lighting);
    packReadDouble(stream, &definition->humidity);

    _validateDefinition(definition);
}

StandardDefinition AtmosphereDefinitionClass = {
    (FuncObjectCreate)_createDefinition,
    (FuncObjectDelete)_deleteDefinition,
    (FuncObjectCopy)_copyDefinition,
    (FuncObjectValidate)_validateDefinition,
    (FuncObjectSave)_saveDefinition,
    (FuncObjectLoad)_loadDefinition
};

/******************** Binding ********************/
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
static int _fakeGetSkydomeLights(Renderer* renderer, LightDefinition* lights, int max_lights)
{
    UNUSED(renderer);
    UNUSED(lights);
    UNUSED(max_lights);
    return 0;
}

static Color _getSkyColor(Renderer* renderer, Vector3 direction)
{
    AtmosphereDefinition* definition;
    double dist;
    Vector3 sun_direction, sun_position;
    Color sky_color, sun_color;

    definition = renderer->atmosphere->definition;

    sun_direction = renderer->atmosphere->getSunDirection(renderer);
    direction = v3Normalize(direction);
    dist = v3Norm(v3Sub(direction, sun_direction));

    sun_position = v3Scale(sun_direction, 149597870.0);

    /* Get base scattering*/
    switch (definition->model)
    {
    case ATMOSPHERE_MODEL_BRUNETON:
        sky_color = brunetonGetSkyColor(definition, renderer->camera_location, direction, sun_position);
        break;
    case ATMOSPHERE_MODEL_PREETHAM:
        sky_color = preethamGetSkyColor(definition, renderer->camera_location, direction, sun_position);
        break;
    default:
        sky_color = COLOR_BLUE;
    }

    /* Get sun halo */
    if (dist < definition->sun_radius)
    {
        sun_color = definition->sun_color;
        sun_color.r *= 100.0;
        sun_color.g *= 100.0;
        sun_color.b *= 100.0;
        if (dist <= definition->sun_radius * 0.9)
        {
            return sun_color;
        }
        else
        {
            sun_color.a = (dist - definition->sun_radius * 0.9) / (definition->sun_radius * 0.1);
            colorMask(&sky_color, &sun_color);
            return sky_color;
        }
    }
    else
    {
        return sky_color;
    }
}

static Vector3 _getSunDirection(Renderer* renderer)
{
    Vector3 result;
    double sun_angle = (renderer->atmosphere->definition->_daytime + 0.75) * M_PI * 2.0;
    result.x = cos(sun_angle);
    result.y = sin(sun_angle);
    result.z = 0.0;
    return result;
}

#if 0
static inline void _addDomeLight(Renderer* renderer, LightDefinition* light, Vector3 direction, double factor)
{
    light->direction = v3Scale(direction, -1.0);
    light->color = renderer->atmosphere->getSkyColor(renderer, direction);
    light->color.r *= factor;
    light->color.g *= factor;
    light->color.b *= factor;
    light->reflection = 0.0;
    light->filtered = 0;
    light->masked = 0;
}

static int _getSkydomeLights(Renderer* renderer, LightDefinition* lights, int max_lights)
{
    AtmosphereRendererCache* cache = (AtmosphereRendererCache*)renderer->atmosphere->_internal_data;
    AtmosphereDefinition* definition;
    double sun_angle;
    Vector3 sun_direction;
    int nblights = 0;

    mutexAcquire(cache->lock);
    if (cache->nblights < 0)
    {
        definition = renderer->atmosphere->definition;

        sun_angle = (definition->_daytime + 0.75) * M_PI * 2.0;
        sun_direction.x = cos(sun_angle);
        sun_direction.y = sin(sun_angle);
        sun_direction.z = 0.0;

        /* TODO Moon light */

        if (max_lights > MAX_SKYDOME_LIGHTS)
        {
            max_lights = MAX_SKYDOME_LIGHTS;
        }

        if (max_lights > 0)
        {
            /* Direct light from the sun */
            cache->lights[0].direction = v3Scale(sun_direction, -1.0);
            cache->lights[0].color = definition->sun_color;
            cache->lights[0].reflection = 1.0;
            cache->lights[0].filtered = 1;
            cache->lights[0].masked = 1;
            nblights = 1;
            max_lights--;
        }

        if (max_lights > 0)
        {
            /* Indirect lighting by skydome scattering */
            int xsamples, ysamples, samples, x, y;
            double xstep, ystep, factor;
            Vector3 direction;

            samples = (renderer->render_quality < 5) ? 9 : (renderer->render_quality * 4 + 1);
            samples = samples > max_lights ? max_lights : samples;

            factor = definition->dome_lighting / (double)samples;

            _addDomeLight(renderer, cache->lights + nblights, VECTOR_UP, factor);
            nblights++;
            samples--;

            if (samples >= 2)
            {
                xsamples = samples / 2;
                ysamples = samples / xsamples;

                xstep = M_PI * 2.0 / (double)xsamples;
                ystep = M_PI * 0.5 / (double)ysamples;

                for (x = 0; x < xsamples; x++)
                {
                    for (y = 0; y < ysamples; y++)
                    {
                        direction.x = cos(x * xstep) * cos(y * ystep);
                        direction.y = -sin(y * ystep);
                        direction.z = sin(x * xstep) * cos(y * ystep);

                        _addDomeLight(renderer, cache->lights + nblights, direction, factor);
                        nblights++;
                    }
                }
            }
        }

        cache->nblights = nblights;
    }
    mutexRelease(cache->lock);

    memcpy(lights, cache->lights, sizeof(LightDefinition) * cache->nblights);
    return cache->nblights;
}
#endif

static void _fakeGetLightingStatus(Renderer* renderer, LightStatus* status, Vector3 normal, int opaque)
{
    UNUSED(renderer);
    UNUSED(status);
    UNUSED(normal);
    UNUSED(opaque);
}

/******************** Renderer ********************/
static AtmosphereRenderer* _createRenderer()
{
    AtmosphereRenderer* result;
    AtmosphereRendererCache* cache;

    result = malloc(sizeof(AtmosphereRenderer));
    result->definition = AtmosphereDefinitionClass.create();

    result->getLightingStatus = _fakeGetLightingStatus;
    result->getSunDirection = _getSunDirection;
    result->applyAerialPerspective = _fakeApplyAerialPerspective;
    result->getSkyColor = _fakeGetSkyColor;

    cache = malloc(sizeof(AtmosphereRendererCache));
    cache->lock = mutexCreate();
    cache->nblights = -1;
    result->_internal_data = cache;

    return result;
}

static void _deleteRenderer(AtmosphereRenderer* renderer)
{
    AtmosphereRendererCache* cache = (AtmosphereRendererCache*)renderer->_internal_data;
    mutexDestroy(cache->lock);
    free(cache);

    AtmosphereDefinitionClass.destroy(renderer->definition);
    free(renderer);
}

static void _bindRenderer(AtmosphereRenderer* renderer, AtmosphereDefinition* definition)
{
    AtmosphereRendererCache* cache = (AtmosphereRendererCache*)renderer->_internal_data;

    AtmosphereDefinitionClass.copy(definition, renderer->definition);

    renderer->getSkyColor = _getSkyColor;
    renderer->getLightingStatus = basicGetLightingStatus;

    switch (definition->model)
    {
        case ATMOSPHERE_MODEL_BRUNETON:
            renderer->applyAerialPerspective = brunetonApplyAerialPerspective;
            break;
        default:
            renderer->applyAerialPerspective = basicApplyAerialPerspective;
    }

    mutexAcquire(cache->lock);
    cache->nblights = -1;
    mutexRelease(cache->lock);
}

StandardRenderer AtmosphereRendererClass = {
    (FuncObjectCreate)_createRenderer,
    (FuncObjectDelete)_deleteRenderer,
    (FuncObjectBind)_bindRenderer
};
