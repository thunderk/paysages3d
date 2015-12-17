#include "WaterRasterizer.h"

#include <cmath>
#include "SoftwareRenderer.h"
#include "WaterRenderer.h"
#include "CanvasFragment.h"
#include "RenderProgress.h"

WaterRasterizer::WaterRasterizer(SoftwareRenderer *renderer, RenderProgress *progress, unsigned short client_id)
    : Rasterizer(renderer, progress, client_id, Color(0.7, 0.85, 1.0)) {
}

static inline Vector3 _getFirstPassVertex(SoftwareRenderer *renderer, double x, double z) {
    Vector3 result;

    result.x = x;
    result.y = renderer->getWaterRenderer()->getHeight(x, z);
    result.z = z;

    return result;
}

void WaterRasterizer::rasterizeQuad(CanvasPortion *canvas, double x, double z, double size) {
    Vector3 v1, v2, v3, v4;

    v1 = _getFirstPassVertex(renderer, x, z);
    v2 = _getFirstPassVertex(renderer, x, z + size);
    v3 = _getFirstPassVertex(renderer, x + size, z + size);
    v4 = _getFirstPassVertex(renderer, x + size, z);

    pushQuad(canvas, v1, v2, v3, v4);
}

int WaterRasterizer::prepareRasterization() {
    return performTessellation(NULL);
}

void WaterRasterizer::rasterizeToCanvas(CanvasPortion *canvas) {
    performTessellation(canvas);
}

Color WaterRasterizer::shadeFragment(const CanvasFragment &fragment, const CanvasFragment *) const {
    Vector3 location = fragment.getLocation();
    return renderer->getWaterRenderer()->getResult(location.x, location.z).final;
}

void WaterRasterizer::setQuality(double factor) {
    base_chunk_size = 10.0 / (1.0 + factor * 7.0);
    if (factor > 0.6) {
        base_chunk_size *= 0.5;
    }
}

int WaterRasterizer::performTessellation(CanvasPortion *canvas) {
    int chunk_factor, chunk_count, i, result;
    Vector3 cam = renderer->getCameraLocation(VECTOR_ZERO);
    double radius_int, radius_ext, chunk_size;

    result = 0;
    chunk_factor = 1;
    chunk_count = 2;
    radius_int = 0.0;
    radius_ext = base_chunk_size;
    chunk_size = base_chunk_size;

    double cx = cam.x - fmod(cam.x, base_chunk_size);
    double cz = cam.z - fmod(cam.x, base_chunk_size);

    while (radius_int < 20000.0) {
        if (interrupted) {
            return result;
        }

        for (i = 0; i < chunk_count - 1; i++) {
            result++;
            if (canvas) {
                rasterizeQuad(canvas, cx - radius_ext + chunk_size * i, cz - radius_ext, chunk_size);
                rasterizeQuad(canvas, cx + radius_int, cz - radius_ext + chunk_size * i, chunk_size);
                rasterizeQuad(canvas, cx + radius_int - chunk_size * i, cz + radius_int, chunk_size);
                rasterizeQuad(canvas, cx - radius_ext, cz + radius_int - chunk_size * i, chunk_size);
            }
        }
        if (canvas) {
            progress->add(chunk_count - 1);
        }

        if (radius_int > 20.0 && chunk_count % 64 == 0 && to_double(chunk_factor) < radius_int / 20.0) {
            chunk_count /= 2;
            chunk_factor *= 2;
        }
        chunk_count += 2;
        chunk_size = base_chunk_size * chunk_factor;
        radius_int = radius_ext;
        radius_ext += chunk_size;
    }

    return result;
}
