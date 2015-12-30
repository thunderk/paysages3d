#include "SkyRasterizer.h"

#include <cmath>
#include "Maths.h"
#include "Vector3.h"
#include "Color.h"
#include "SoftwareRenderer.h"
#include "AtmosphereRenderer.h"
#include "AtmosphereResult.h"
#include "CloudsRenderer.h"
#include "Rasterizer.h"
#include "CanvasFragment.h"
#include "RenderProgress.h"
#include "GodRaysSampler.h"

#define SPHERE_SIZE 20000.0

SkyRasterizer::SkyRasterizer(SoftwareRenderer *renderer, RenderProgress *progress, unsigned short client_id)
    : Rasterizer(renderer, progress, client_id, Color(0.7, 0.7, 1.0)) {
}

int SkyRasterizer::prepareRasterization() {
    return res_i * res_j;
}

void SkyRasterizer::rasterizeToCanvas(CanvasPortion *canvas) {
    int i, j;
    double step_i, step_j;
    double current_i, current_j;
    Vector3 vertex1, vertex2, vertex3, vertex4;
    Vector3 camera_location, direction;

    step_i = Maths::PI * 2.0 / to_double(res_i);
    step_j = Maths::PI / to_double(res_j);

    camera_location = renderer->getCameraLocation(VECTOR_ZERO);

    for (j = 0; j < res_j; j++) {
        if (interrupted) {
            return;
        }

        current_j = to_double(j - res_j / 2) * step_j;

        for (i = 0; i < res_i; i++) {
            current_i = to_double(i) * step_i;

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

            // TODO Triangles at poles
            pushQuad(canvas, vertex1, vertex4, vertex3, vertex2);
        }
        progress->add(res_i);
    }
}

Color SkyRasterizer::shadeFragment(const CanvasFragment &fragment, const CanvasFragment *) const {
    Vector3 location = fragment.getLocation();
    Vector3 camera_location, direction;
    Color result;

    camera_location = renderer->getCameraLocation(location);
    direction = location.sub(camera_location);

    // TODO Don't compute sky color if it's fully covered by clouds
    result = renderer->getAtmosphereRenderer()->getSkyColor(direction.normalize()).final;
    result =
        renderer->getCloudsRenderer()->getColor(camera_location, camera_location.add(direction.scale(10.0)), result);

    return result;
}

void SkyRasterizer::setQuality(int res_i, int res_j) {
    this->res_i = res_i;
    this->res_j = res_j;
}

void SkyRasterizer::setQuality(double factor) {
    setQuality(20.0 * (1.0 + factor * 10.0), 10.0 * (1.0 + factor * 10.0));
}
