#include "public.h"
#include "private.h"

#include <stdlib.h>
#include "../renderer.h"
#include "../tools.h"

static Color _postProcessFragment(Renderer* renderer, Vector3 location, void* data)
{
    UNUSED(data);
    return renderer->water->getResult(renderer, location.x, location.z).final;
}

static Vector3 _getFirstPassVertex(Renderer* renderer, double x, double z)
{
    Vector3 result;

    result.x = x;
    result.y = renderer->water->getHeight(renderer, x, z);
    result.z = z;

    return result;
}

static void _renderQuad(Renderer* renderer, double x, double z, double size)
{
    Vector3 v1, v2, v3, v4;

    v1 = _getFirstPassVertex(renderer, x, z);
    v2 = _getFirstPassVertex(renderer, x, z + size);
    v3 = _getFirstPassVertex(renderer, x + size, z + size);
    v4 = _getFirstPassVertex(renderer, x + size, z);

    renderer->pushQuad(renderer, v1, v2, v3, v4, _postProcessFragment, NULL);
}

void waterRenderSurface(Renderer* renderer)
{
    int chunk_factor, chunk_count, i;
    Vector3 cam = renderer->getCameraLocation(renderer, VECTOR_ZERO);
    double cx = cam.x;
    double cz = cam.z;
    double radius_int, radius_ext, base_chunk_size, chunk_size;

    base_chunk_size = 2.0 / (double)renderer->render_quality;
    if (renderer->render_quality > 7)
    {
        base_chunk_size *= 0.5;
    }

    chunk_factor = 1;
    chunk_count = 2;
    radius_int = 0.0;
    radius_ext = base_chunk_size;
    chunk_size = base_chunk_size;

    while (radius_int < 20000.0)
    {
        if (!renderer->addRenderProgress(renderer, 0.0))
        {
            return;
        }

        for (i = 0; i < chunk_count - 1; i++)
        {
            _renderQuad(renderer, cx - radius_ext + chunk_size * i, cz - radius_ext, chunk_size);
            _renderQuad(renderer, cx + radius_int, cz - radius_ext + chunk_size * i, chunk_size);
            _renderQuad(renderer, cx + radius_int - chunk_size * i, cz + radius_int, chunk_size);
            _renderQuad(renderer, cx - radius_ext, cz + radius_int - chunk_size * i, chunk_size);
        }

        if (radius_int > 20.0 && chunk_count % 64 == 0 && (double)chunk_factor < radius_int / 20.0)
        {
            chunk_count /= 2;
            chunk_factor *= 2;
        }
        chunk_count += 2;
        chunk_size = base_chunk_size * chunk_factor;
        radius_int = radius_ext;
        radius_ext += chunk_size;
    }
}
