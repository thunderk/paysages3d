#include "TerrainRasterizer.h"

#include "SoftwareRenderer.h"
#include "BoundingBox.h"
#include "CameraDefinition.h"
#include "TerrainRenderer.h"
#include "WaterRenderer.h"
#include "TexturesRenderer.h"
#include "Scenery.h"
#include "CanvasPortion.h"
#include "CanvasFragment.h"

TerrainRasterizer::TerrainRasterizer(SoftwareRenderer* renderer, int client_id):
    Rasterizer(renderer, client_id, Color(1.0, 0.9, 0.9))
{
}

static inline Vector3 _getPoint(SoftwareRenderer* renderer, double x, double z)
{
    return Vector3(x, renderer->getTerrainRenderer()->getHeight(x, z, 1), z);
}

void TerrainRasterizer::tessellateChunk(CanvasPortion* canvas, TerrainChunkInfo* chunk, int detail)
{
    if (detail < 1)
    {
        return;
    }

    double water_height = renderer->getWaterRenderer()->getHeightInfo().min_height;

    double startx = chunk->point_nw.x;
    double startz = chunk->point_nw.z;
    double size = (chunk->point_ne.x - chunk->point_nw.x) / (double)detail;
    int i, j;

    for (i = 0; i < detail; i++)
    {
        for (j = 0; j < detail; j++)
        {
            renderQuad(canvas, startx + (double)i * size, startz + (double)j * size, size, water_height);
        }
    }
}

void TerrainRasterizer::renderQuad(CanvasPortion *canvas, double x, double z, double size, double water_height)
{
    Vector3 ov1, ov2, ov3, ov4;
    Vector3 dv1, dv2, dv3, dv4;

    ov1.y = ov2.y = ov3.y = ov4.y = 0.0;

    ov1.x = x;
    ov1.z = z;
    dv1 = renderer->getTerrainRenderer()->getResult(x, z, 1, 1).location;

    ov2.x = x;
    ov2.z = z + size;
    dv2 = renderer->getTerrainRenderer()->getResult(x, z + size, 1, 1).location;

    ov3.x = x + size;
    ov3.z = z + size;
    dv3 = renderer->getTerrainRenderer()->getResult(x + size, z + size, 1, 1).location;

    ov4.x = x + size;
    ov4.z = z;
    dv4 = renderer->getTerrainRenderer()->getResult(x + size, z, 1, 1).location;

    if (dv1.y > water_height || dv2.y > water_height || dv3.y > water_height || dv4.y > water_height)
    {
        pushDisplacedQuad(canvas, dv1, dv2, dv3, dv4, ov1, ov2, ov3, ov4);
    }
}

static void _getChunk(SoftwareRenderer* renderer, TerrainRasterizer::TerrainChunkInfo* chunk, double x, double z, double size, int displaced)
{
    chunk->point_nw = renderer->getTerrainRenderer()->getResult(x, z, 1, displaced).location;
    chunk->point_sw = renderer->getTerrainRenderer()->getResult(x, z + size, 1, displaced).location;
    chunk->point_se = renderer->getTerrainRenderer()->getResult(x + size, z + size, 1, displaced).location;
    chunk->point_ne = renderer->getTerrainRenderer()->getResult(x + size, z, 1, displaced).location;

    double displacement_power;
    if (displaced)
    {
        displacement_power = 0.0;
    }
    else
    {
        displacement_power = renderer->getTexturesRenderer()->getMaximalDisplacement(renderer->getScenery()->getTextures());
    }

    BoundingBox box;
    if (displacement_power > 0.0)
    {
        box.pushPoint(chunk->point_nw.add(Vector3(-displacement_power, displacement_power, -displacement_power)));
        box.pushPoint(chunk->point_nw.add(Vector3(-displacement_power, -displacement_power, -displacement_power)));
        box.pushPoint(chunk->point_sw.add(Vector3(-displacement_power, displacement_power, displacement_power)));
        box.pushPoint(chunk->point_sw.add(Vector3(-displacement_power, -displacement_power, displacement_power)));
        box.pushPoint(chunk->point_se.add(Vector3(displacement_power, displacement_power, displacement_power)));
        box.pushPoint(chunk->point_se.add(Vector3(displacement_power, -displacement_power, displacement_power)));
        box.pushPoint(chunk->point_ne.add(Vector3(displacement_power, displacement_power, -displacement_power)));
        box.pushPoint(chunk->point_ne.add(Vector3(displacement_power, -displacement_power, -displacement_power)));
    }
    else
    {
        box.pushPoint(chunk->point_nw);
        box.pushPoint(chunk->point_sw);
        box.pushPoint(chunk->point_se);
        box.pushPoint(chunk->point_ne);
    }

    int coverage = renderer->render_camera->isUnprojectedBoxInView(box);
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

void TerrainRasterizer::getTessellationInfo(CanvasPortion* canvas, int displaced)
{
    TerrainChunkInfo chunk;
    int chunk_factor, chunk_count, i;
    Vector3 cam = renderer->getCameraLocation(VECTOR_ZERO);
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
            if (!processChunk(canvas, &chunk, progress))
            {
                return;
            }

            _getChunk(renderer, &chunk, cx + radius_int, cz - radius_ext + chunk_size * i, chunk_size, displaced);
            if (!processChunk(canvas, &chunk, progress))
            {
                return;
            }

            _getChunk(renderer, &chunk, cx + radius_int - chunk_size * i, cz + radius_int, chunk_size, displaced);
            if (!processChunk(canvas, &chunk, progress))
            {
                return;
            }

            _getChunk(renderer, &chunk, cx - radius_ext, cz + radius_int - chunk_size * i, chunk_size, displaced);
            if (!processChunk(canvas, &chunk, progress))
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

int TerrainRasterizer::processChunk(CanvasPortion* canvas, TerrainChunkInfo* chunk, double progress)
{
    tessellateChunk(canvas, chunk, chunk->detail_hint);

    renderer->render_progress = 0.05 * progress;
    return !renderer->render_interrupt;
}

void TerrainRasterizer::rasterizeToCanvas(CanvasPortion *canvas)
{
    renderer->render_progress = 0.0;
    getTessellationInfo(canvas, 0);
    renderer->render_progress = 0.05;
}

Color TerrainRasterizer::shadeFragment(const CanvasFragment &fragment) const
{
    Vector3 point = fragment.getLocation();
    double precision = renderer->getPrecision(_getPoint(renderer, point.x, point.z));
    return renderer->getTerrainRenderer()->getFinalColor(point, precision);
}
