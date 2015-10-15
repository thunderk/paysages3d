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
#include "RenderProgress.h"

TerrainRasterizer::TerrainRasterizer(SoftwareRenderer* renderer, RenderProgress *progress, int client_id):
    Rasterizer(renderer, progress, client_id, Color(1.0, 0.9, 0.9))
{
}

void TerrainRasterizer::setQuality(double base_chunk_size, double detail_factor, int max_chunk_detail)
{
    this->base_chunk_size = base_chunk_size;
    this->detail_factor = detail_factor;
    this->max_chunk_detail = max_chunk_detail;
}

void TerrainRasterizer::setQuality(double factor)
{
    setQuality(5.0 - 4.5 * factor * factor,
               1.0 / (25.0 - 20.0 * factor),
               1 + 49 * factor * factor);
}

static inline Vector3 _getPoint(SoftwareRenderer* renderer, double x, double z)
{
    return Vector3(x, renderer->getTerrainRenderer()->getHeight(x, z, true), z);
}

void TerrainRasterizer::tessellateChunk(CanvasPortion* canvas, TerrainChunkInfo* chunk, int detail)
{
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
        progress->add(detail);
    }
}

void TerrainRasterizer::renderQuad(CanvasPortion *canvas, double x, double z, double size, double water_height)
{
    Vector3 ov1, ov2, ov3, ov4;
    Vector3 dv1, dv2, dv3, dv4;

    ov1.y = ov2.y = ov3.y = ov4.y = 0.0;

    ov1.x = x;
    ov1.z = z;
    dv1 = renderer->getTerrainRenderer()->getResult(x, z, true, true).location;

    ov2.x = x;
    ov2.z = z + size;
    dv2 = renderer->getTerrainRenderer()->getResult(x, z + size, true, true).location;

    ov3.x = x + size;
    ov3.z = z + size;
    dv3 = renderer->getTerrainRenderer()->getResult(x + size, z + size, true, true).location;

    ov4.x = x + size;
    ov4.z = z;
    dv4 = renderer->getTerrainRenderer()->getResult(x + size, z, true, true).location;

    if (dv1.y > water_height || dv2.y > water_height || dv3.y > water_height || dv4.y > water_height)
    {
        pushDisplacedQuad(canvas, dv1, dv2, dv3, dv4, ov1, ov2, ov3, ov4);
    }
}

void TerrainRasterizer::getChunk(TerrainRasterizer::TerrainChunkInfo* chunk, double x, double z, double size, bool displaced)
{
    chunk->point_nw = renderer->getTerrainRenderer()->getResult(x, z, true, displaced).location;
    chunk->point_sw = renderer->getTerrainRenderer()->getResult(x, z + size, true, displaced).location;
    chunk->point_se = renderer->getTerrainRenderer()->getResult(x + size, z + size, true, displaced).location;
    chunk->point_ne = renderer->getTerrainRenderer()->getResult(x + size, z, true, displaced).location;

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
        chunk->detail_hint = (int)ceil(sqrt((double)coverage) * detail_factor);
        if (chunk->detail_hint > max_chunk_detail)
        {
            chunk->detail_hint = max_chunk_detail;
        }
    }
    else
    {
        /* Not in view */
        chunk->detail_hint = -1;
    }
}

int TerrainRasterizer::performTessellation(CanvasPortion* canvas, bool displaced)
{
    TerrainChunkInfo chunk;
    int chunk_factor, chunk_count, i, result;
    Vector3 cam = renderer->getCameraLocation(VECTOR_ZERO);
    double radius_int, radius_ext;
    double chunk_size;

    chunk_factor = 1;
    chunk_count = 2;
    radius_int = 0.0;
    radius_ext = base_chunk_size;
    chunk_size = base_chunk_size;
    result = 0;

    double cx = cam.x - fmod(cam.x, base_chunk_size);
    double cz = cam.z - fmod(cam.x, base_chunk_size);

    while (radius_int < 20000.0)
    {
        for (i = 0; i < chunk_count - 1; i++)
        {
            getChunk(&chunk, cx - radius_ext + chunk_size * i, cz - radius_ext, chunk_size, displaced);
            if (chunk.detail_hint > 0)
            {
                result += chunk.detail_hint * chunk.detail_hint;
                if (canvas)
                {
                    processChunk(canvas, &chunk);
                }
            }
            if (interrupted)
            {
                return result;
            }

            getChunk(&chunk, cx + radius_int, cz - radius_ext + chunk_size * i, chunk_size, displaced);
            if (chunk.detail_hint > 0)
            {
                result += chunk.detail_hint * chunk.detail_hint;
                if (canvas)
                {
                    processChunk(canvas, &chunk);
                }
            }
            if (interrupted)
            {
                return result;
            }

            getChunk(&chunk, cx + radius_int - chunk_size * i, cz + radius_int, chunk_size, displaced);
            if (chunk.detail_hint > 0)
            {
                result += chunk.detail_hint * chunk.detail_hint;
                if (canvas)
                {
                    processChunk(canvas, &chunk);
                }
            }
            if (interrupted)
            {
                return result;
            }

            getChunk(&chunk, cx - radius_ext, cz + radius_int - chunk_size * i, chunk_size, displaced);
            if (chunk.detail_hint > 0)
            {
                result += chunk.detail_hint * chunk.detail_hint;
                if (canvas)
                {
                    processChunk(canvas, &chunk);
                }
            }
            if (interrupted)
            {
                return result;
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

    return result;
}

void TerrainRasterizer::processChunk(CanvasPortion* canvas, TerrainChunkInfo* chunk)
{
    tessellateChunk(canvas, chunk, chunk->detail_hint);
}

int TerrainRasterizer::prepareRasterization()
{
    // TODO Chunks could be saved and reused in rasterizeToCanvas
    return performTessellation(NULL, false);
}

void TerrainRasterizer::rasterizeToCanvas(CanvasPortion *canvas)
{
    performTessellation(canvas, false);
}

Color TerrainRasterizer::shadeFragment(const CanvasFragment &fragment, const CanvasFragment *) const
{
    Vector3 point = fragment.getLocation();
    double precision = renderer->getPrecision(_getPoint(renderer, point.x, point.z));
    return renderer->getTerrainRenderer()->getFinalColor(point, precision);
}
