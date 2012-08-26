#include "sky.h"

#include <stdlib.h>
#include <math.h>

#include "shared/types.h"
#include "color.h"
#include "clouds.h"
#include "euclid.h"
#include "lighting.h"
#include "render.h"
#include "tools.h"
#include "skypreetham.h"
#include "skyrayleigh.h"

#define SPHERE_SIZE 1000.0

/******************************** Configuration ********************************/
void skySave(PackStream* stream, SkyDefinition* definition)
{
    packWriteInt(stream, (int*)&definition->model);
    packWriteDouble(stream, &definition->daytime);
    colorSave(stream, &definition->sun_color);
    packWriteDouble(stream, &definition->sun_radius);
    packWriteDouble(stream, &definition->sun_halo_size);
    curveSave(stream, definition->sun_halo_profile);
    packWriteDouble(stream, &definition->dome_lighting);
    
    packWriteInt(stream, &definition->model_custom.auto_from_daytime);
    colorSave(stream, &definition->model_custom.zenith_color);
    colorSave(stream, &definition->model_custom.haze_color);
    packWriteDouble(stream, &definition->model_custom.haze_height);
    packWriteDouble(stream, &definition->model_custom.haze_smoothing);
    
    packWriteDouble(stream, &definition->model_preetham.turbidity);
}

void skyLoad(PackStream* stream, SkyDefinition* definition)
{
    packReadInt(stream, (int*)&definition->model);
    packReadDouble(stream, &definition->daytime);
    colorLoad(stream, &definition->sun_color);
    packReadDouble(stream, &definition->sun_radius);
    packReadDouble(stream, &definition->sun_halo_size);
    curveLoad(stream, definition->sun_halo_profile);
    packReadDouble(stream, &definition->dome_lighting);
    
    packReadInt(stream, &definition->model_custom.auto_from_daytime);
    colorLoad(stream, &definition->model_custom.zenith_color);
    colorLoad(stream, &definition->model_custom.haze_color);
    packReadDouble(stream, &definition->model_custom.haze_height);
    packReadDouble(stream, &definition->model_custom.haze_smoothing);
    
    packReadDouble(stream, &definition->model_preetham.turbidity);

    skyValidateDefinition(definition);
}

SkyDefinition skyCreateDefinition()
{
    SkyDefinition def;

    def.model = SKY_MODEL_CUSTOM;
    def.daytime = 0.0;
    def.sun_color = COLOR_BLACK;
    def.sun_radius = 1.0;
    def.sun_halo_size = 0.0;
    def.sun_halo_profile = curveCreate();
    def.dome_lighting = 0.0;
    def.model_custom.auto_from_daytime = 0;
    def.model_custom.zenith_color = COLOR_BLACK;
    def.model_custom.haze_color = COLOR_BLACK;
    def.model_custom.haze_height = 0.0;
    def.model_custom.haze_smoothing = 0.0;
    def.model_custom._sky_gradation = colorGradationCreate();
    def.model_preetham.turbidity = 0.0;

    skyValidateDefinition(&def);

    return def;
}

void skyDeleteDefinition(SkyDefinition* definition)
{
    curveDelete(definition->sun_halo_profile);
    colorGradationDelete(definition->model_custom._sky_gradation);
}

void skyCopyDefinition(SkyDefinition* source, SkyDefinition* destination)
{
    destination->model = source->model;
    destination->daytime = source->daytime;
    destination->sun_color = source->sun_color;
    destination->sun_radius = source->sun_radius;
    destination->sun_halo_size = source->sun_halo_size;
    destination->dome_lighting = source->dome_lighting;
    destination->model_custom.auto_from_daytime = source->model_custom.auto_from_daytime;
    destination->model_custom.zenith_color = source->model_custom.zenith_color;
    destination->model_custom.haze_color = source->model_custom.haze_color;
    destination->model_custom.haze_height = source->model_custom.haze_height;
    destination->model_custom.haze_smoothing = source->model_custom.haze_smoothing;
    destination->model_preetham.turbidity = source->model_preetham.turbidity;
    
    curveCopy(source->sun_halo_profile, destination->sun_halo_profile);
    
    skyValidateDefinition(destination);
}

static void _setAutoCustomModel(SkyDefinition* definition)
{
    ColorGradation* zenith_gradation;
    ColorGradation* haze_gradation;
    
    zenith_gradation = colorGradationCreate();
    haze_gradation = colorGradationCreate();

    colorGradationQuickAddRgb(zenith_gradation, 0.2, 0.03, 0.03, 0.05);
    colorGradationQuickAddRgb(zenith_gradation, 0.25, 0.25, 0.33, 0.37);
    colorGradationQuickAddRgb(zenith_gradation, 0.35, 0.52, 0.63, 0.8);
    colorGradationQuickAddRgb(zenith_gradation, 0.65, 0.52, 0.63, 0.8);
    colorGradationQuickAddRgb(zenith_gradation, 0.75, 0.25, 0.33, 0.37);
    colorGradationQuickAddRgb(zenith_gradation, 0.8, 0.03, 0.03, 0.05);
    colorGradationQuickAddRgb(haze_gradation, 0.2, 0.05, 0.05, 0.08);
    colorGradationQuickAddRgb(haze_gradation, 0.25, 0.55, 0.42, 0.42);
    colorGradationQuickAddRgb(haze_gradation, 0.3, 0.6, 0.6, 0.6);
    colorGradationQuickAddRgb(haze_gradation, 0.4, 0.92, 0.93, 1.0);
    colorGradationQuickAddRgb(haze_gradation, 0.6, 0.92, 0.93, 1.0);
    colorGradationQuickAddRgb(haze_gradation, 0.7, 0.6, 0.6, 0.8);
    colorGradationQuickAddRgb(haze_gradation, 0.75, 0.62, 0.50, 0.42);
    colorGradationQuickAddRgb(haze_gradation, 0.8, 0.05, 0.05, 0.08);

    definition->model_custom.zenith_color = colorGradationGet(zenith_gradation, definition->daytime);
    definition->model_custom.haze_color = colorGradationGet(haze_gradation, definition->daytime);
    
    colorGradationDelete(zenith_gradation);
    colorGradationDelete(haze_gradation);
}

void skyValidateDefinition(SkyDefinition* definition)
{
    if (definition->model == SKY_MODEL_CUSTOM)
    {
        if (definition->model_custom.auto_from_daytime)
        {
            _setAutoCustomModel(definition);
        }
        colorGradationClear(definition->model_custom._sky_gradation);
        colorGradationQuickAdd(definition->model_custom._sky_gradation, 0.0, &definition->model_custom.haze_color);
        colorGradationQuickAdd(definition->model_custom._sky_gradation, definition->model_custom.haze_height - definition->model_custom.haze_smoothing, &definition->model_custom.haze_color);
        colorGradationQuickAdd(definition->model_custom._sky_gradation, definition->model_custom.haze_height, &definition->model_custom.zenith_color);
        colorGradationQuickAdd(definition->model_custom._sky_gradation, 1.0, &definition->model_custom.zenith_color);
    }
}

static inline void _addDomeLight(SkyDefinition* sky, Renderer* renderer, LightDefinition* light, Vector3 direction, double factor)
{
    light->direction = v3Scale(direction, -1.0);
    light->color = skyGetColor(sky, renderer, VECTOR_ZERO, direction);
    light->color.r *= factor;
    light->color.g *= factor;
    light->color.b *= factor;
    light->reflection = 0.0;
    light->filtered = 0;
    light->masked = 0;
}

int skyGetLights(SkyDefinition* sky, Renderer* renderer, LightDefinition* lights, int max_lights)
{
    double sun_angle;
    Vector3 sun_direction;
    int nblights = 0;

    sun_angle = (sky->daytime + 0.75) * M_PI * 2.0;
    sun_direction.x = cos(sun_angle);
    sun_direction.y = sin(sun_angle);
    sun_direction.z = 0.0;

    /* TODO Moon light */

    if (max_lights > 0)
    {
        /* Direct light from the sun */
        lights[0].direction = v3Scale(sun_direction, -1.0);
        lights[0].color = sky->sun_color;
        lights[0].reflection = 1.0;
        lights[0].filtered = 1;
        lights[0].masked = 1;
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
        
        factor = sky->dome_lighting / (double)samples;
        
        _addDomeLight(sky, renderer, lights + nblights, VECTOR_UP, factor);
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

                    _addDomeLight(sky, renderer, lights + nblights, direction, factor);
                    nblights++;
                }
            }
        }
    }

    return nblights;
}

/******************************** Rendering ********************************/
Color skyGetColor(SkyDefinition* definition, Renderer* renderer, Vector3 eye, Vector3 look)
{
    double dist;
    Vector3 sun_position;
    Color sun_color, sky_color;
    
    sun_position = skyGetSunDirection(definition);

    look = v3Normalize(look);
    dist = v3Norm(v3Sub(look, sun_position));

    if (definition->model == SKY_MODEL_PREETHAM)
    {
        sky_color = skyPreethamGetColor(eye, look, sun_position, definition->model_preetham.turbidity);
    }
    else if (definition->model == SKY_MODEL_RAYLEIGH_MIE)
    {
        sky_color = skyRayleighGetColor(eye, look, sun_position);
    }
    else
    {
        sky_color = colorGradationGet(definition->model_custom._sky_gradation, look.y * 0.5 + 0.5);
    }
    
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

static Color _postProcessFragment(Renderer* renderer, Vector3 location, void* data)
{
    Vector3 direction;
    Color result;
    SkyDefinition* definition;

    definition = (SkyDefinition*)data;

    direction = v3Sub(location, renderer->camera_location);

    result = skyGetColor(definition, renderer, renderer->camera_location, v3Normalize(direction));
    result = renderer->applyClouds(renderer, result, renderer->camera_location, v3Add(renderer->camera_location, v3Scale(direction, 10.0)));

    return result;
}

void skyRender(SkyDefinition* definition, Renderer* renderer)
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
            renderer->pushQuad(renderer, vertex1, vertex4, vertex3, vertex2, _postProcessFragment, definition);
        }
    }
}

Vector3 skyGetSunDirection(SkyDefinition* definition)
{
    Vector3 result;
    double sun_angle = (definition->daytime + 0.75) * M_PI * 2.0;
    result.x = cos(sun_angle);
    result.y = sin(sun_angle);
    result.z = 0.0;
    return result;
}

Color skyGetSunColor(SkyDefinition* definition)
{
    return definition->sun_color;
}

Color skyGetZenithColor(SkyDefinition* definition)
{
    Vector3 look = {0.0, 1.0, 0.0};
    return skyGetColor(definition, NULL, VECTOR_ZERO, look);
}

Color skyGetHorizonColor(SkyDefinition* definition)
{
    Vector3 look = {0.0, 0.0, 1.0};
    return skyGetColor(definition, NULL, VECTOR_ZERO, look);
}
