#include "public.h"
#include "private.h"

#include <stdlib.h>
#include "../renderer.h"

/*
 * Terrain rasterization.
 */

static inline Vector3 _getPoint(TerrainDefinition* definition, Renderer* renderer, double x, double z)
{
    Vector3 result;

    result.x = x;
    result.y = renderer->terrain->getHeight(renderer, x, z, 1);
    result.z = z;

    return result;
}

static Color _postProcessFragment(Renderer* renderer, Vector3 point, void* data)
{
    double precision;

    point = _getPoint(renderer->terrain->definition, renderer, point.x, point.z);

    precision = renderer->getPrecision(renderer, point);
    return renderer->terrain->getFinalColor(renderer, point, precision);
}

static void _renderQuad(Renderer* renderer, double x, double z, double size, double water_height)
{
    Vector3 ov1, ov2, ov3, ov4;
    Vector3 dv1, dv2, dv3, dv4;

    ov1.y = ov2.y = ov3.y = ov4.y;

    ov1.x = x;
    ov1.z = z;
    dv1 = renderer->terrain->getResult(renderer, x, z, 1, 1).location;

    ov2.x = x;
    ov2.z = z + size;
    dv2 = renderer->terrain->getResult(renderer, x, z + size, 1, 1).location;

    ov3.x = x + size;
    ov3.z = z + size;
    dv3 = renderer->terrain->getResult(renderer, x + size, z + size, 1, 1).location;

    ov4.x = x + size;
    ov4.z = z;
    dv4 = renderer->terrain->getResult(renderer, x + size, z, 1, 1).location;

    if (dv1.y > water_height || dv2.y > water_height || dv3.y > water_height || dv4.y > water_height)
    {
        renderer->pushDisplacedQuad(renderer, dv1, dv2, dv3, dv4, ov1, ov2, ov3, ov4, _postProcessFragment, NULL);
    }
}

void terrainRenderSurface(Renderer* renderer)
{
    int chunk_factor, chunk_count, i;
    Vector3 cam = renderer->getCameraLocation(renderer, VECTOR_ZERO);
    double cx = cam.x;
    double cz = cam.z;
    double min_chunk_size, visible_chunk_size;
    double radius_int, radius_ext, chunk_size;
    double water_height;

    min_chunk_size = 0.2 / (double)renderer->render_quality;
    visible_chunk_size = 0.1 / (double)renderer->render_quality;
    if (renderer->render_quality > 7)
    {
        min_chunk_size *= 0.25;
        visible_chunk_size *= 0.25;
    }

    chunk_factor = 1;
    chunk_count = 2;
    radius_int = 0.0;
    radius_ext = min_chunk_size;
    chunk_size = min_chunk_size;

    water_height = renderer->water->getHeightInfo(renderer).max_height;

    while (radius_int < 5000.0)
    {
        if (!renderer->addRenderProgress(renderer, 0.0))
        {
            return;
        }

        for (i = 0; i < chunk_count - 1; i++)
        {
            _renderQuad(renderer, cx - radius_ext + chunk_size * i, cz - radius_ext, chunk_size, water_height);
            _renderQuad(renderer, cx + radius_int, cz - radius_ext + chunk_size * i, chunk_size, water_height);
            _renderQuad(renderer, cx + radius_int - chunk_size * i, cz + radius_int, chunk_size, water_height);
            _renderQuad(renderer, cx - radius_ext, cz + radius_int - chunk_size * i, chunk_size, water_height);
        }

        if (chunk_count % 64 == 0 && chunk_size / radius_int < visible_chunk_size)
        {
            chunk_count /= 2;
            chunk_factor *= 2;
            /* TODO Fill in gaps with triangles */
        }
        chunk_count += 2;
        chunk_size = min_chunk_size * chunk_factor;
        radius_int = radius_ext;
        radius_ext += chunk_size;
    }
}