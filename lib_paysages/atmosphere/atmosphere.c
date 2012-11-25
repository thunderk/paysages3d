#include "atmosphere.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "../tools.h"
#include "../renderer.h"
#include "../lighting.h"
#include "../system.h"

#define SPHERE_SIZE 1000.0
#define MAX_SKYDOME_LIGHTS 100

typedef struct
{
    Mutex* lock;
    int nblights;
    LightDefinition lights[MAX_SKYDOME_LIGHTS];
} AtmosphereRendererCache;

/******************** Definition ********************/
static void _validateDefinition(AtmosphereDefinition* definition)
{
    UNUSED(definition);
}

static AtmosphereDefinition* _createDefinition()
{
    AtmosphereDefinition* result;

    result = malloc(sizeof(AtmosphereDefinition));
    result->model = ATMOSPHERE_MODEL_PREETHAM;
    result->daytime = 0.0;
    result->sun_color = COLOR_BLACK;
    result->sun_color.r = 1.0;
    result->sun_color.g = 0.95;
    result->sun_color.b = 0.9;
    result->sun_color.a = 1.0;
    result->sun_radius = 0.02;
    result->sun_halo_size = 0.3;
    result->sun_halo_profile = curveCreate();
    curveQuickAddPoint(result->sun_halo_profile, 0.0, 1.0);
    curveQuickAddPoint(result->sun_halo_profile, 0.1, 0.2);
    curveQuickAddPoint(result->sun_halo_profile, 1.0, 0.0);
    result->dome_lighting = 0.6;
    result->humidity = 2.0;
    
    _validateDefinition(result);

    return result;
}

static void _deleteDefinition(AtmosphereDefinition* definition)
{
    curveDelete(definition->sun_halo_profile);
    free(definition);
}

static void _copyDefinition(AtmosphereDefinition* source, AtmosphereDefinition* destination)
{
    destination->model = source->model;
    destination->daytime = source->daytime;
    destination->sun_color = source->sun_color;
    destination->sun_radius = source->sun_radius;
    destination->sun_halo_size = source->sun_halo_size;
    destination->dome_lighting = source->dome_lighting;
    destination->humidity = source->humidity;
    
    curveCopy(source->sun_halo_profile, destination->sun_halo_profile);
    
    _validateDefinition(destination);
}

static void _saveDefinition(PackStream* stream, AtmosphereDefinition* definition)
{
    packWriteInt(stream, (int*)&definition->model);
    packWriteDouble(stream, &definition->daytime);
    colorSave(stream, &definition->sun_color);
    packWriteDouble(stream, &definition->sun_radius);
    packWriteDouble(stream, &definition->sun_halo_size);
    curveSave(stream, definition->sun_halo_profile);
    packWriteDouble(stream, &definition->dome_lighting);
    packWriteDouble(stream, &definition->humidity);
}

static void _loadDefinition(PackStream* stream, AtmosphereDefinition* definition)
{
    packReadInt(stream, (int*)&definition->model);
    packReadDouble(stream, &definition->daytime);
    colorLoad(stream, &definition->sun_color);
    packReadDouble(stream, &definition->sun_radius);
    packReadDouble(stream, &definition->sun_halo_size);
    curveLoad(stream, definition->sun_halo_profile);
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
extern Color brunetonGetSkyColor(AtmosphereDefinition* definition, Vector3 eye, Vector3 direction, Vector3 sun_position);
extern Color preethamGetSkyColor(AtmosphereDefinition* definition, Vector3 eye, Vector3 direction, Vector3 sun_position);

static Color _fakeApplyAerialPerspective(Renderer* renderer, Vector3 direction, Color base)
{
    UNUSED(renderer);
    UNUSED(direction);
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
    Vector3 sun_position;
    Color sky_color, sun_color;
    
    definition = renderer->atmosphere->definition;
    
    sun_position = renderer->atmosphere->getSunDirection(renderer);
    direction = v3Normalize(direction);
    dist = v3Norm(v3Sub(direction, sun_position));

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
    if (dist < definition->sun_radius + definition->sun_halo_size)
    {
        sun_color = definition->sun_color;
        if (dist <= definition->sun_radius)
        {
            return sun_color;
        }
        else
        {
            dist = (dist - definition->sun_radius) / definition->sun_halo_size;
            sun_color.a = curveGetValue(definition->sun_halo_profile, dist);
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
    double sun_angle = (renderer->atmosphere->definition->daytime + 0.75) * M_PI * 2.0;
    result.x = cos(sun_angle);
    result.y = sin(sun_angle);
    result.z = 0.0;
    return result;
}

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

        sun_angle = (definition->daytime + 0.75) * M_PI * 2.0;
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

/******************** Renderer ********************/
static AtmosphereRenderer* _createRenderer()
{
    AtmosphereRenderer* result;
    AtmosphereRendererCache* cache;
    
    result = malloc(sizeof(AtmosphereRenderer));
    result->definition = AtmosphereDefinitionClass.create();
    
    result->getSunDirection = _getSunDirection;
    result->applyAerialPerspective = _fakeApplyAerialPerspective;
    result->getSkydomeLights = _fakeGetSkydomeLights;
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
    renderer->getSkydomeLights = _getSkydomeLights;
    
    mutexAcquire(cache->lock);
    cache->nblights = -1;
    mutexRelease(cache->lock);
}

StandardRenderer AtmosphereRendererClass = {
    (FuncObjectCreate)_createRenderer,
    (FuncObjectDelete)_deleteRenderer,
    (FuncObjectBind)_bindRenderer
};

/******************** Utilities ********************/
static Color _postProcessFragment(Renderer* renderer, Vector3 location, void* data)
{
    Vector3 direction;
    Color result;
    
    UNUSED(data);

    direction = v3Sub(location, renderer->camera_location);

    /* TODO Don't compute result->color if it's fully covered by clouds */
    result = renderer->atmosphere->getSkyColor(renderer, v3Normalize(direction));
    result = renderer->applyClouds(renderer, result, renderer->camera_location, v3Add(renderer->camera_location, v3Scale(direction, 10.0)));

    return result;
}

void atmosphereRenderSkydome(Renderer* renderer)
{
    int res_i, res_j;
    int i, j;
    double step_i, step_j;
    double current_i, current_j;
    Vector3 vertex1, vertex2, vertex3, vertex4;
    Vector3 direction;

    res_i = renderer->render_quality * 40;
    res_j = renderer->render_quality * 20;
    step_i = M_PI * 2.0 / (double)res_i;
    step_j = M_PI / (double)res_j;

    for (j = 0; j < res_j; j++)
    {
        if (!renderer->addRenderProgress(renderer, 0.0))
        {
            return;
        }

        current_j = (double)(j - res_j / 2) * step_j;

        for (i = 0; i < res_i; i++)
        {
            current_i = (double)i * step_i;

            direction.x = SPHERE_SIZE * cos(current_i) * cos(current_j);
            direction.y = SPHERE_SIZE * sin(current_j);
            direction.z = SPHERE_SIZE * sin(current_i) * cos(current_j);
            vertex1 = v3Add(renderer->camera_location, direction);

            direction.x = SPHERE_SIZE * cos(current_i + step_i) * cos(current_j);
            direction.y = SPHERE_SIZE * sin(current_j);
            direction.z = SPHERE_SIZE * sin(current_i + step_i) * cos(current_j);
            vertex2 = v3Add(renderer->camera_location, direction);

            direction.x = SPHERE_SIZE * cos(current_i + step_i) * cos(current_j + step_j);
            direction.y = SPHERE_SIZE * sin(current_j + step_j);
            direction.z = SPHERE_SIZE * sin(current_i + step_i) * cos(current_j + step_j);
            vertex3 = v3Add(renderer->camera_location, direction);

            direction.x = SPHERE_SIZE * cos(current_i) * cos(current_j + step_j);
            direction.y = SPHERE_SIZE * sin(current_j + step_j);
            direction.z = SPHERE_SIZE * sin(current_i) * cos(current_j + step_j);
            vertex4 = v3Add(renderer->camera_location, direction);

            /* TODO Triangles at poles */
            renderer->pushQuad(renderer, vertex1, vertex4, vertex3, vertex2, _postProcessFragment, NULL);
        }
    }
}
