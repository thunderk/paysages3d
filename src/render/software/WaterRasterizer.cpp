#include "WaterRasterizer.h"

#include "SoftwareRenderer.h"
#include "WaterRenderer.h"
#include "ParallelQueue.h"

WaterRasterizer::WaterRasterizer(SoftwareRenderer* renderer):
    renderer(renderer)
{
}

static Color _postProcessFragment(SoftwareRenderer* renderer, Vector3 location, void*)
{
    return renderer->getWaterRenderer()->getResult(location.x, location.z).final;
}

static Vector3 _getFirstPassVertex(SoftwareRenderer* renderer, double x, double z)
{
    Vector3 result;

    result.x = x;
    result.y = renderer->getWaterRenderer()->getHeight(x, z);
    result.z = z;

    return result;
}

static void _renderQuad(SoftwareRenderer* renderer, double x, double z, double size)
{
    Vector3 v1, v2, v3, v4;

    v1 = _getFirstPassVertex(renderer, x, z);
    v2 = _getFirstPassVertex(renderer, x, z + size);
    v3 = _getFirstPassVertex(renderer, x + size, z + size);
    v4 = _getFirstPassVertex(renderer, x + size, z);

    renderer->pushQuad(v1, v2, v3, v4, _postProcessFragment, NULL);
}

typedef struct
{
    SoftwareRenderer* renderer;
    int i;
    double cx;
    double cz;
    double radius_int;
    double chunk_size;
    double radius_ext;
} ParallelRasterInfo;

static int _parallelJobCallback(ParallelQueue*, int, void* data, int stopping)
{
    ParallelRasterInfo* info = (ParallelRasterInfo*)data;

    if (!stopping)
    {
        _renderQuad(info->renderer, info->cx - info->radius_ext + info->chunk_size * info->i, info->cz - info->radius_ext, info->chunk_size);
        _renderQuad(info->renderer, info->cx + info->radius_int, info->cz - info->radius_ext + info->chunk_size * info->i, info->chunk_size);
        _renderQuad(info->renderer, info->cx + info->radius_int - info->chunk_size * info->i, info->cz + info->radius_int, info->chunk_size);
        _renderQuad(info->renderer, info->cx - info->radius_ext, info->cz + info->radius_int - info->chunk_size * info->i, info->chunk_size);
    }

    free(data);
    return 0;
}

void WaterRasterizer::renderSurface()
{
    ParallelRasterInfo* info;
    ParallelQueue* queue;
    queue = new ParallelQueue();

    int chunk_factor, chunk_count, i;
    Vector3 cam = renderer->getCameraLocation(VECTOR_ZERO);
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

    double cx = cam.x - fmod(cam.x, base_chunk_size);
    double cz = cam.z - fmod(cam.x, base_chunk_size);

    while (radius_int < 20000.0)
    {
        if (!renderer->addRenderProgress(0.0))
        {
            return;
        }

        for (i = 0; i < chunk_count - 1; i++)
        {
            info = new ParallelRasterInfo;

            info->renderer = renderer;
            info->cx = cx;
            info->cz = cz;
            info->i = i;
            info->radius_int = radius_int;
            info->radius_ext = radius_ext;
            info->chunk_size = chunk_size;

            if (!queue->addJob(_parallelJobCallback, info))
            {
                delete info;
            }
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

    queue->wait();
    delete queue;
}