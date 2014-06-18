#include "SkyRasterizer.h"

#include "Vector3.h"
#include "Color.h"
#include "SoftwareRenderer.h"
#include "AtmosphereRenderer.h"
#include "AtmosphereResult.h"
#include "CloudsRenderer.h"
#include "Rasterizer.h"

#define SPHERE_SIZE 20000.0

SkyRasterizer::SkyRasterizer(SoftwareRenderer* renderer, int client_id):
    Rasterizer(renderer, client_id)
{
}

static Color _postProcessFragment(SoftwareRenderer* renderer, const Vector3 &location, void*)
{
    Vector3 camera_location, direction;
    Color result;

    camera_location = renderer->getCameraLocation(location);
    direction = location.sub(camera_location);

    /* TODO Don't compute result->color if it's fully covered by clouds */
    result = renderer->getAtmosphereRenderer()->getSkyColor(direction.normalize()).final;
    result = renderer->getCloudsRenderer()->getColor(camera_location, camera_location.add(direction.scale(10.0)), result);

    return result;
}

void SkyRasterizer::rasterizeToCanvas(CanvasPortion* canvas)
{
    int res_i, res_j;
    int i, j;
    double step_i, step_j;
    double current_i, current_j;
    Vector3 vertex1, vertex2, vertex3, vertex4;
    Vector3 camera_location, direction;

    res_i = renderer->render_quality * 40;
    res_j = renderer->render_quality * 20;
    step_i = M_PI * 2.0 / (double)res_i;
    step_j = M_PI / (double)res_j;

    camera_location = renderer->getCameraLocation(VECTOR_ZERO);

    for (j = 0; j < res_j; j++)
    {
        if (!renderer->addRenderProgress(0.0))
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
            vertex1 = camera_location.add(direction);

            direction.x = SPHERE_SIZE * cos(current_i + step_i) * cos(current_j);
            direction.y = SPHERE_SIZE * sin(current_j);
            direction.z = SPHERE_SIZE * sin(current_i + step_i) * cos(current_j);
            vertex2 = camera_location.add(direction);

            direction.x = SPHERE_SIZE * cos(current_i + step_i) * cos(current_j + step_j);
            direction.y = SPHERE_SIZE * sin(current_j + step_j);
            direction.z = SPHERE_SIZE * sin(current_i + step_i) * cos(current_j + step_j);
            vertex3 = camera_location.add(direction);

            direction.x = SPHERE_SIZE * cos(current_i) * cos(current_j + step_j);
            direction.y = SPHERE_SIZE * sin(current_j + step_j);
            direction.z = SPHERE_SIZE * sin(current_i) * cos(current_j + step_j);
            vertex4 = camera_location.add(direction);

            /* TODO Triangles at poles */
            pushQuad(canvas, vertex1, vertex4, vertex3, vertex2);
        }
    }
}
