#include "sky.h"

#include <stdlib.h>
#include <math.h>

#include "shared/types.h"
#include "shared/globals.h"
#include "shared/constants.h"
#include "color.h"
#include "clouds.h"
#include "euclid.h"
#include "lighting.h"
#include "render.h"
#include "tools.h"

#define SPHERE_SIZE 1000.0

void skyInit()
{
}

void skySave(FILE* f, SkyDefinition* definition)
{
    toolsSaveDouble(f, &definition->daytime);
    colorGradationSave(f, &definition->sun_color);
    toolsSaveDouble(f, &definition->sun_radius);
    colorGradationSave(f, &definition->zenith_color);
    colorGradationSave(f, &definition->haze_color);
    toolsSaveDouble(f, &definition->haze_height);
    toolsSaveDouble(f, &definition->haze_smoothing);
}

void skyLoad(FILE* f, SkyDefinition* definition)
{
    toolsLoadDouble(f, &definition->daytime);
    colorGradationLoad(f, &definition->sun_color);
    toolsLoadDouble(f, &definition->sun_radius);
    colorGradationLoad(f, &definition->zenith_color);
    colorGradationLoad(f, &definition->haze_color);
    toolsLoadDouble(f, &definition->haze_height);
    toolsLoadDouble(f, &definition->haze_smoothing);

    skyValidateDefinition(definition);
}

SkyDefinition skyCreateDefinition()
{
    SkyDefinition def;

    def.daytime = 0.0;
    def.sun_color = colorGradationCreate();
    def.sun_radius = 1.0;
    def.zenith_color = colorGradationCreate();
    def.haze_color = colorGradationCreate();
    def.haze_height = 0.0;
    def.haze_smoothing = 0.0;

    skyValidateDefinition(&def);

    return def;
}

void skyDeleteDefinition(SkyDefinition* definition)
{
}

void skyCopyDefinition(SkyDefinition* source, SkyDefinition* destination)
{
    *destination = *source;
}

void skyValidateDefinition(SkyDefinition* definition)
{
    Color zenith, haze;

    zenith = colorGradationGet(&definition->zenith_color, definition->daytime);
    haze = colorGradationGet(&definition->haze_color, definition->daytime);

    definition->_sky_gradation = colorGradationCreate();
    colorGradationAdd(&definition->_sky_gradation, 0.0, &haze);
    colorGradationAdd(&definition->_sky_gradation, definition->haze_height - definition->haze_smoothing, &haze);
    colorGradationAdd(&definition->_sky_gradation, definition->haze_height, &zenith);
    colorGradationAdd(&definition->_sky_gradation, 1.0, &zenith);
}

int skyGetLights(SkyDefinition* sky, LightDefinition* lights, int max_lights)
{
    double sun_angle;
    Vector3 sun_direction;
    int nblights = 0;

    sun_angle = (sky->daytime + 0.75) * M_PI * 2.0;
    sun_direction.x = cos(sun_angle);
    sun_direction.y = sin(sun_angle);
    sun_direction.z = 0.0;

    /* TODO Night lights */

    if (max_lights > 0)
    {
        /* Light from the sun */
        lights[0].direction = v3Scale(sun_direction, -1.0);
        lights[0].color = colorGradationGet(&sky->sun_color, sky->daytime);
        lights[0].reflection = 1.0;
        lights[0].filtered = 1;
        lights[0].masked = 1;
        lights[0].amplitude = 0.0;
        nblights = 1;
        if (max_lights > 1)
        {
            /* Skydome lighting */
            lights[1].direction.x = 0.0;
            lights[1].direction.y = -1.0;
            lights[1].direction.z = 0.0;
            lights[1].color = colorGradationGet(&sky->zenith_color, sky->daytime);
            lights[1].color.r *= 0.4;
            lights[1].color.g *= 0.4;
            lights[1].color.b *= 0.4;
            lights[1].reflection = 0.0;
            lights[1].filtered = 1;
            lights[1].masked = 0;
            lights[1].amplitude = M_PI / 2.0;
            nblights = 2;
        }
    }

    return nblights;
}

Color skyGetColor(SkyDefinition* definition, Renderer* renderer, Vector3 eye, Vector3 look)
{
    double sun_angle, dist;
    Vector3 sun_position;
    Color sun_color, sky_color;

    sun_angle = (definition->daytime + 0.75) * M_PI * 2.0;
    sun_position.x = cos(sun_angle);
    sun_position.y = sin(sun_angle);
    sun_position.z = 0.0;

    look = v3Normalize(look);
    dist = v3Norm(v3Sub(look, sun_position));

    sky_color = colorGradationGet(&definition->_sky_gradation, look.y * 0.5 + 0.5);
    if (dist < definition->sun_radius)
    {
        dist = dist / definition->sun_radius;
        sun_color = colorGradationGet(&definition->sun_color, definition->daytime);
        if (dist < 0.9)
        {
            return sun_color;
        }
        else
        {
            sun_color.a = (1.0 - dist) / 0.1;
            colorMask(&sky_color, &sun_color);
            return sky_color;
        }
    }
    else
    {
        return sky_color;
    }
}

static int _postProcessFragment(RenderFragment* fragment, Renderer* renderer, void* data)
{
    Vector3 location, direction;
    Color result;
    SkyDefinition* definition;

    definition = (SkyDefinition*)data;

    location = fragment->vertex.location;
    direction = v3Sub(location, renderer->camera_location);

    result = skyGetColor(definition, renderer, renderer->camera_location, v3Normalize(direction));
    result = renderer->applyClouds(renderer, result, renderer->camera_location, v3Add(renderer->camera_location, v3Scale(direction, 10.0)));

    fragment->vertex.color = result;

    return 1;
}

void skyRender(SkyDefinition* definition, Renderer* renderer, RenderProgressCallback callback)
{
    int res_i, res_j;
    int i, j;
    double step_i, step_j;
    double current_i, current_j;
    Vertex vertex1, vertex2, vertex3, vertex4;
    Color col;
    Vector3 direction;

    res_i = renderer->render_quality * 40;
    res_j = renderer->render_quality * 20;
    step_i = M_PI * 2.0 / (double)res_i;
    step_j = M_PI / (double)res_j;

    col.r = 0.0;
    col.g = 0.0;
    col.a = 1.0;

    for (j = 0; j < res_j; j++)
    {
        if (!callback((double)j / (double)(res_j - 1)))
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
            vertex1.location = v3Add(renderer->camera_location, direction);
            col.b = sin(direction.x) * sin(direction.x) * cos(direction.z) * cos(direction.z);
            vertex1.color = col;
            vertex1.callback = _postProcessFragment;
            vertex1.callback_data = definition;

            direction.x = SPHERE_SIZE * cos(current_i + step_i) * cos(current_j);
            direction.y = SPHERE_SIZE * sin(current_j);
            direction.z = SPHERE_SIZE * sin(current_i + step_i) * cos(current_j);
            vertex2.location = v3Add(renderer->camera_location, direction);
            col.b = sin(direction.x) * sin(direction.x) * cos(direction.z) * cos(direction.z);
            vertex2.color = col;
            vertex2.callback = _postProcessFragment;
            vertex2.callback_data = definition;

            direction.x = SPHERE_SIZE * cos(current_i + step_i) * cos(current_j + step_j);
            direction.y = SPHERE_SIZE * sin(current_j + step_j);
            direction.z = SPHERE_SIZE * sin(current_i + step_i) * cos(current_j + step_j);
            vertex3.location = v3Add(renderer->camera_location, direction);
            col.b = sin(direction.x) * sin(direction.x) * cos(direction.z) * cos(direction.z);
            vertex3.color = col;
            vertex3.callback = _postProcessFragment;
            vertex3.callback_data = definition;

            direction.x = SPHERE_SIZE * cos(current_i) * cos(current_j + step_j);
            direction.y = SPHERE_SIZE * sin(current_j + step_j);
            direction.z = SPHERE_SIZE * sin(current_i) * cos(current_j + step_j);
            vertex4.location = v3Add(renderer->camera_location, direction);
            col.b = sin(direction.x) * sin(direction.x) * cos(direction.z) * cos(direction.z);
            vertex4.color = col;
            vertex4.callback = _postProcessFragment;
            vertex4.callback_data = definition;

            /* TODO Triangles at poles */
            renderPushQuad(renderer, &vertex1, &vertex4, &vertex3, &vertex2);
        }
    }
}
