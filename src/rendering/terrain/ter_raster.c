#include "public.h"
#include "private.h"
#include "ter_raster.h"

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
    /* TODO Tessellate ! */
    _renderQuad(renderer, chunk->point_nw.x, chunk->point_nw.z, chunk->point_ne.x - chunk->point_nw.x, renderer->water->getHeightInfo(renderer).min_height);
}

static void _getChunk(Renderer* renderer, TerrainChunkInfo* chunk, double x, double z, double size, double water_height, int displaced)
{
    chunk->point_nw = renderer->terrain->getResult(renderer, x, z, 1, displaced).location;
    chunk->point_sw = renderer->terrain->getResult(renderer, x, z + size, 1, displaced).location;
    chunk->point_se = renderer->terrain->getResult(renderer, x + size, z + size, 1, displaced).location;
    chunk->point_ne = renderer->terrain->getResult(renderer, x + size, z, 1, displaced).location;

    /* TODO If displaced == 0, the detail_hint may be inaccurate (water and screen culling) */

    if (chunk->point_nw.y >= water_height || chunk->point_sw.y >= water_height || chunk->point_se.y >= water_height || chunk->point_ne.y >= water_height)
    {
        /* TODO */
        chunk->detail_hint = 5;
    }
    else
    {
        chunk->detail_hint = -1;
    }
}

void terrainGetTessellationInfo(Renderer* renderer, FuncTerrainTessellationCallback callback, int displaced)
{
    TerrainChunkInfo chunk;
    int chunk_factor, chunk_count, i;
    Vector3 cam = renderer->getCameraLocation(renderer, VECTOR_ZERO);
    double cx = cam.x;
    double cz = cam.z;
    double water_height;
    double progress;
    double radius_int, radius_ext;
    double base_chunk_size, chunk_size;

    base_chunk_size = 5.0 / (double)renderer->render_quality;

    chunk_factor = 1;
    chunk_count = 2;
    radius_int = 0.0;
    radius_ext = base_chunk_size;
    chunk_size = base_chunk_size;
    progress = 0.0; /* TODO */

    water_height = renderer->water->getHeightInfo(renderer).min_height;

    while (radius_int < 5000.0)
    {
        for (i = 0; i < chunk_count - 1; i++)
        {
            _getChunk(renderer, &chunk, cx - radius_ext + chunk_size * i, cz - radius_ext, chunk_size, water_height, displaced);
            if (!callback(renderer, &chunk, progress))
            {
                return;
            }

            _getChunk(renderer, &chunk, cx + radius_int, cz - radius_ext + chunk_size * i, chunk_size, water_height, displaced);
            if (!callback(renderer, &chunk, progress))
            {
                return;
            }

            _getChunk(renderer, &chunk, cx + radius_int - chunk_size * i, cz + radius_int, chunk_size, water_height, displaced);
            if (!callback(renderer, &chunk, progress))
            {
                return;
            }

            _getChunk(renderer, &chunk, cx - radius_ext, cz + radius_int - chunk_size * i, chunk_size, water_height, displaced);
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

static int _standardTessellationCallback(Renderer* renderer, TerrainChunkInfo* chunk, double progress)
{
    terrainTessellateChunk(renderer, chunk, chunk->detail_hint);
    return !renderer->render_interrupt;
}

void terrainRenderSurface(Renderer* renderer)
{
    terrainGetTessellationInfo(renderer, _standardTessellationCallback, 0);
}
