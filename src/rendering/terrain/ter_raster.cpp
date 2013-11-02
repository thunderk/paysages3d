#include "public.h"
#include "private.h"
#include "ter_raster.h"

#include <stdlib.h>
#include <math.h>
#include "../tools.h"
#include "../tools/boundingbox.h"
#include "../tools/parallel.h"
#include "../renderer.h"

/*
 * Terrain rasterization.
 */

static inline Vector3 _getPoint(TerrainDefinition* definition, Renderer* renderer, double x, double z)
{
    UNUSED(definition);

    Vector3 result;

    result.x = x;
    result.y = renderer->terrain->getHeight(renderer, x, z, 1);
    result.z = z;

    return result;
}

static Color _postProcessFragment(Renderer* renderer, Vector3 point, void*)
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

    ov1.y = ov2.y = ov3.y = ov4.y = 0.0;

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

void terrainTessellateChunk(Renderer* renderer, TerrainChunkInfo* chunk, int detail)
{
    if (detail < 1)
    {
        return;
    }

    double water_height = renderer->water->getHeightInfo(renderer).min_height;

    double startx = chunk->point_nw.x;
    double startz = chunk->point_nw.z;
    double size = (chunk->point_ne.x - chunk->point_nw.x) / (double)detail;
    int i, j;

    for (i = 0; i < detail; i++)
    {
        for (j = 0; j < detail; j++)
        {
            _renderQuad(renderer, startx + (double)i * size, startz + (double)j * size, size, water_height);
        }
    }
}

static void _getChunk(Renderer* renderer, TerrainChunkInfo* chunk, double x, double z, double size, int displaced)
{
    chunk->point_nw = renderer->terrain->getResult(renderer, x, z, 1, displaced).location;
    chunk->point_sw = renderer->terrain->getResult(renderer, x, z + size, 1, displaced).location;
    chunk->point_se = renderer->terrain->getResult(renderer, x + size, z + size, 1, displaced).location;
    chunk->point_ne = renderer->terrain->getResult(renderer, x + size, z, 1, displaced).location;

    double displacement_power;
    if (displaced)
    {
        displacement_power = 0.0;
    }
    else
    {
        displacement_power = texturesGetMaximalDisplacement(renderer->textures->definition);
    }

    BoundingBox box;
    boundingBoxReset(&box);
    if (displacement_power > 0.0)
    {
        boundingBoxPushPoint(&box, v3Add(chunk->point_nw, v3(-displacement_power, displacement_power, -displacement_power)));
        boundingBoxPushPoint(&box, v3Add(chunk->point_nw, v3(-displacement_power, -displacement_power, -displacement_power)));
        boundingBoxPushPoint(&box, v3Add(chunk->point_sw, v3(-displacement_power, displacement_power, displacement_power)));
        boundingBoxPushPoint(&box, v3Add(chunk->point_sw, v3(-displacement_power, -displacement_power, displacement_power)));
        boundingBoxPushPoint(&box, v3Add(chunk->point_se, v3(displacement_power, displacement_power, displacement_power)));
        boundingBoxPushPoint(&box, v3Add(chunk->point_se, v3(displacement_power, -displacement_power, displacement_power)));
        boundingBoxPushPoint(&box, v3Add(chunk->point_ne, v3(displacement_power, displacement_power, -displacement_power)));
        boundingBoxPushPoint(&box, v3Add(chunk->point_ne, v3(displacement_power, -displacement_power, -displacement_power)));
    }
    else
    {
        boundingBoxPushPoint(&box, chunk->point_nw);
        boundingBoxPushPoint(&box, chunk->point_sw);
        boundingBoxPushPoint(&box, chunk->point_se);
        boundingBoxPushPoint(&box, chunk->point_ne);
    }

    int coverage = cameraIsUnprojectedBoxInView(renderer->render_camera, &box);
    if (coverage > 0)
    {
        chunk->detail_hint = (int)ceil(sqrt((double)coverage) / (double)(25 - 2 * renderer->render_quality));
        if (chunk->detail_hint > 5 * renderer->render_quality)
        {
            chunk->detail_hint = 5 * renderer->render_quality;
        }
    }
    else
    {
        /* Not in view */
        chunk->detail_hint = -1;
    }
}

void terrainGetTessellationInfo(Renderer* renderer, FuncTerrainTessellationCallback callback, int displaced)
{
    TerrainChunkInfo chunk;
    int chunk_factor, chunk_count, i;
    Vector3 cam = renderer->getCameraLocation(renderer, VECTOR_ZERO);
    double progress;
    double radius_int, radius_ext;
    double base_chunk_size, chunk_size;

    base_chunk_size = 5.0 / (double)renderer->render_quality;

    chunk_factor = 1;
    chunk_count = 2;
    radius_int = 0.0;
    radius_ext = base_chunk_size;
    chunk_size = base_chunk_size;
    progress = 0.0;

    double cx = cam.x - fmod(cam.x, base_chunk_size);
    double cz = cam.z - fmod(cam.x, base_chunk_size);

    while (radius_int < 20000.0)
    {
        progress = radius_int / 20000.0;
        for (i = 0; i < chunk_count - 1; i++)
        {
            _getChunk(renderer, &chunk, cx - radius_ext + chunk_size * i, cz - radius_ext, chunk_size, displaced);
            if (!callback(renderer, &chunk, progress))
            {
                return;
            }

            _getChunk(renderer, &chunk, cx + radius_int, cz - radius_ext + chunk_size * i, chunk_size, displaced);
            if (!callback(renderer, &chunk, progress))
            {
                return;
            }

            _getChunk(renderer, &chunk, cx + radius_int - chunk_size * i, cz + radius_int, chunk_size, displaced);
            if (!callback(renderer, &chunk, progress))
            {
                return;
            }

            _getChunk(renderer, &chunk, cx - radius_ext, cz + radius_int - chunk_size * i, chunk_size, displaced);
            if (!callback(renderer, &chunk, progress))
            {
                return;
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
}

typedef struct
{
    Renderer* renderer;
    TerrainChunkInfo chunk;
} ParallelRasterInfo;

static int _parallelJobCallback(ParallelQueue* queue, int job_id, void* data, int stopping)
{
    ParallelRasterInfo* info = (ParallelRasterInfo*)data;
    UNUSED(queue);
    UNUSED(job_id);

    if (!stopping)
    {
        terrainTessellateChunk(info->renderer, &info->chunk, info->chunk.detail_hint);
    }

    free(data);
    return 0;
}

static int _standardTessellationCallback(Renderer* renderer, TerrainChunkInfo* chunk, double progress)
{
    ParallelRasterInfo* info = new ParallelRasterInfo;

    info->renderer = renderer;
    info->chunk = *chunk;

    if (!parallelQueueAddJob((ParallelQueue*)renderer->customData[0], _parallelJobCallback, info))
    {
        delete info;
    }

    renderer->render_progress = 0.05 * progress;
    return !renderer->render_interrupt;
}

void terrainRenderSurface(Renderer* renderer)
{
    ParallelQueue* queue;
    queue = parallelQueueCreate(0);

    /* TODO Do not use custom data, it could already be used by another module */
    renderer->customData[0] = queue;

    renderer->render_progress = 0.0;
    terrainGetTessellationInfo(renderer, _standardTessellationCallback, 0);
    renderer->render_progress = 0.05;

    parallelQueueWait(queue);
    parallelQueueDelete(queue);
}
