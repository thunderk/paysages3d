#include <stdlib.h>
#include <math.h>

#include "shared/types.h"
#include "shared/functions.h"
#include "shared/globals.h"
#include "shared/constants.h"
#include "clouds.h"

ColorGradation _gradation;
#define SPHERE_SIZE 1000.0

void skySave(FILE* f)
{
}

void skyLoad(FILE* f)
{
}

Color skyGetColor(Vector3 start, Vector3 direction)
{
    direction = v3Normalize(direction);
    return colorGradationGet(&_gradation, direction.y * 0.5 + 0.5);
}

Color skyProjectRay(Vector3 start, Vector3 direction)
{
    Color color_sky, color_clouds;

    direction = v3Normalize(direction);
    
    color_sky = skyGetColor(start, direction);
    color_clouds = cloudsGetColor(start, v3Add(start, v3Scale(direction, SPHERE_SIZE)));
    
    colorMask(&color_sky, &color_clouds);

    return color_sky;
}

void skySetGradation(ColorGradation grad)
{
    _gradation = grad;
}

static int _postProcessFragment(RenderFragment* fragment)
{
    Vector3 location, direction;
    Color color_sky, color_clouds;

    location = fragment->vertex.location;
    direction = v3Sub(location, camera_location);

    color_sky = skyGetColor(camera_location, v3Normalize(direction));
    color_clouds = cloudsGetColor(camera_location, v3Add(camera_location, v3Scale(direction, 10.0)));
    
    colorMask(&color_sky, &color_clouds);
    fragment->vertex.color = color_sky;

    return 1;
}

void skyRender(RenderProgressCallback callback)
{
    int res_i, res_j;
    int i, j;
    double step_i, step_j;
    double current_i, current_j;
    Vertex vertex1, vertex2, vertex3, vertex4;
    Color col;
    Vector3 direction;

    res_i = render_quality * 20;
    res_j = render_quality * 10;
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
            vertex1.location = v3Add(camera_location, direction);
            col.b = sin(direction.x) * sin(direction.x) * cos(direction.z) * cos(direction.z);
            vertex1.color = col;
            vertex1.callback = _postProcessFragment;

            direction.x = SPHERE_SIZE * cos(current_i + step_i) * cos(current_j);
            direction.y = SPHERE_SIZE * sin(current_j);
            direction.z = SPHERE_SIZE * sin(current_i + step_i) * cos(current_j);
            vertex2.location = v3Add(camera_location, direction);
            col.b = sin(direction.x) * sin(direction.x) * cos(direction.z) * cos(direction.z);
            vertex2.color = col;
            vertex2.callback = _postProcessFragment;

            direction.x = SPHERE_SIZE * cos(current_i + step_i) * cos(current_j + step_j);
            direction.y = SPHERE_SIZE * sin(current_j + step_j);
            direction.z = SPHERE_SIZE * sin(current_i + step_i) * cos(current_j + step_j);
            vertex3.location = v3Add(camera_location, direction);
            col.b = sin(direction.x) * sin(direction.x) * cos(direction.z) * cos(direction.z);
            vertex3.color = col;
            vertex3.callback = _postProcessFragment;

            direction.x = SPHERE_SIZE * cos(current_i) * cos(current_j + step_j);
            direction.y = SPHERE_SIZE * sin(current_j + step_j);
            direction.z = SPHERE_SIZE * sin(current_i) * cos(current_j + step_j);
            vertex4.location = v3Add(camera_location, direction);
            col.b = sin(direction.x) * sin(direction.x) * cos(direction.z) * cos(direction.z);
            vertex4.color = col;
            vertex4.callback = _postProcessFragment;

            /* TODO Triangles at poles */
            renderPushQuad(&vertex1, &vertex4, &vertex3, &vertex2);
        }
    }
}
