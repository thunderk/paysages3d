#include "private.h"

/*
 * Terrain height map painting.
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../tools/memory.h"
#include "../tools.h"

TerrainHeightMap* terrainHeightMapCreate(TerrainDefinition* terrain)
{
    TerrainHeightMap* result;

    result = malloc(sizeof(TerrainHeightMap));
    result->terrain = terrain;
    result->fixed_count = 0;
    result->fixed_data = malloc(sizeof(TerrainHeightMapChunk));
    result->floating_used = 0;
    result->floating_data.data = malloc(sizeof(double));

    return result;
}

void terrainHeightmapDelete(TerrainHeightMap* heightmap)
{
    int i;
    for (i = 0; i < heightmap->fixed_count; i++)
    {
        free(heightmap->fixed_data[i].data);
    }
    free(heightmap->fixed_data);
    free(heightmap->floating_data.data);
    free(heightmap);
}

static void _setFixedCount(TerrainHeightMap* heightmap, int new_count)
{
    int old_count = heightmap->fixed_count;
    int i;

    if (new_count > old_count)
    {
        heightmap->fixed_data = realloc(heightmap->fixed_data, sizeof(TerrainHeightMapChunk) * new_count);
        for (i = old_count; i < new_count; i++)
        {
            heightmap->fixed_data[i].data = malloc(sizeof(double));
        }
    }
    else if (new_count < old_count)
    {
        for (i = new_count; i < old_count; i++)
        {
            free(heightmap->fixed_data[i].data);
        }
        if (new_count > 0)
        {
            heightmap->fixed_data = realloc(heightmap->fixed_data, sizeof(TerrainHeightMapChunk) * new_count);
        }
    }

    heightmap->fixed_count = new_count;
}

void terrainHeightmapCopy(TerrainHeightMap* source, TerrainHeightMap* destination)
{
    int i;

    destination->terrain = source->terrain;

    _setFixedCount(destination, source->fixed_count);

    for (i = 0; i < destination->fixed_count; i++)
    {
        TerrainHeightMapChunk* chunk_source = source->fixed_data + i;
        TerrainHeightMapChunk* chunk_destination = destination->fixed_data + i;
        size_t mapsize = sizeof(double) * chunk_source->rect.xsize * chunk_source->rect.zsize;

        chunk_destination->rect = chunk_source->rect;
        if (chunk_source->rect.xsize * chunk_source->rect.zsize > 0)
        {
            chunk_destination->data = realloc(chunk_destination->data, mapsize);
            memcpy(chunk_destination->data, chunk_source->data, mapsize);
        }
    }

    destination->floating_used = 0;
}

void terrainHeightmapSave(PackStream* stream, TerrainHeightMap* heightmap)
{
    int i, j;

    packWriteInt(stream, &heightmap->fixed_count);
    for (i = 0; i < heightmap->fixed_count; i++)
    {
        TerrainHeightMapChunk* chunk = heightmap->fixed_data + i;

        packWriteInt(stream, &chunk->rect.xstart);
        packWriteInt(stream, &chunk->rect.xend);
        packWriteInt(stream, &chunk->rect.xsize);
        packWriteInt(stream, &chunk->rect.zstart);
        packWriteInt(stream, &chunk->rect.zend);
        packWriteInt(stream, &chunk->rect.zsize);
        for (j = 0; j < chunk->rect.xsize * chunk->rect.zsize; j++)
        {
            packWriteDouble(stream, &chunk->data[j]);
        }
    }
}

void terrainHeightmapLoad(PackStream* stream, TerrainHeightMap* heightmap)
{
    int new_count, i, j;

    packReadInt(stream, &new_count);
    _setFixedCount(heightmap, new_count);

    for (i = 0; i < heightmap->fixed_count; i++)
    {
        TerrainHeightMapChunk* chunk = heightmap->fixed_data + i;

        packReadInt(stream, &chunk->rect.xstart);
        packReadInt(stream, &chunk->rect.xend);
        packReadInt(stream, &chunk->rect.xsize);
        packReadInt(stream, &chunk->rect.zstart);
        packReadInt(stream, &chunk->rect.zend);
        packReadInt(stream, &chunk->rect.zsize);
        if (chunk->rect.xsize * chunk->rect.zsize > 0)
        {
            chunk->data = realloc(chunk->data, sizeof(double) * chunk->rect.xsize * chunk->rect.zsize);
        }
        for (j = 0; j < chunk->rect.xsize * chunk->rect.zsize; j++)
        {
            packReadDouble(stream, &chunk->data[j]);
        }
    }

    heightmap->floating_used = 0;
}

static inline int _checkDataHit(TerrainHeightMapChunk* chunk, double x, double z, double* result)
{
    if ((int)floor(x) >= chunk->rect.xstart && (int)floor(x) <= chunk->rect.xend && (int)floor(z) >= (double)chunk->rect.zstart && (int)floor(z) <= (double)chunk->rect.zend)
    {
        double stencil[16];
        int ix, iz, cx, cz;
        int xmax = chunk->rect.xsize - 1;
        int zmax = chunk->rect.zsize - 1;
        int xlow;
        int zlow;

        x -= chunk->rect.xstart;
        z -= chunk->rect.zstart;

        xlow = floor(x);
        zlow = floor(z);

        for (ix = xlow - 1; ix <= xlow + 2; ix++)
        {
            for (iz = zlow - 1; iz <= zlow + 2; iz++)
            {
                cx = ix < 0 ? 0 : ix;
                cx = cx > xmax ? xmax : cx;
                cz = iz < 0 ? 0 : iz;
                cz = cz > zmax ? zmax : cz;
                stencil[(iz - (zlow - 1)) * 4 + ix - (xlow - 1)] = chunk->data[cz * chunk->rect.xsize + cx];
            }
        }

        *result = toolsBicubicInterpolate(stencil, x - (double)xlow, z - (double)zlow);

        return 1;
    }
    else
    {
        return 0;
    }
}

int terrainHeightmapGetHeight(TerrainHeightMap* heightmap, double x, double z, double* result)
{
    int i;

    if (heightmap->floating_used && _checkDataHit(&heightmap->floating_data, x, z, result))
    {
        return 1;
    }
    else
    {
        for (i = heightmap->fixed_count - 1; i >= 0; i--)
        {
            if (_checkDataHit(heightmap->fixed_data + i, x, z, result))
            {
                return 1;
            }
        }
        return 0;
    }
}

static inline void _resetRect(TerrainHeightMap* heightmap, int x1, int x2, int z1, int z2)
{
    int i, x, z;
    double result;

    for (x = x1; x <= x2; x++)
    {
        for (z = z1; z <= z2; z++)
        {
            /* Search for a previous fixed chunk */
            for (i = heightmap->fixed_count - 1; i >= 0; i--)
            {
                if (_checkDataHit(heightmap->fixed_data + i, x + heightmap->floating_data.rect.xstart, z + heightmap->floating_data.rect.zstart, &result))
                {
                    heightmap->floating_data.data[z * heightmap->floating_data.rect.xsize + x] = result;
                    break;
                }
            }
            if (i < 0)
            {
                /* No fixed chunk found */
                heightmap->floating_data.data[z * heightmap->floating_data.rect.xsize + x] = terrainGetGridHeight(heightmap->terrain, x + heightmap->floating_data.rect.xstart, z + heightmap->floating_data.rect.zstart, 0);
            }
        }
    }
}

static inline IntegerRect _getBrushRect(TerrainBrush* brush)
{
    IntegerRect result;
    double s = brush->smoothed_size + brush->hard_radius;

    result.xstart = (int)floor(brush->relative_x - s);
    result.xend = (int)ceil(brush->relative_x + s);
    result.zstart = (int)floor(brush->relative_z - s);
    result.zend = (int)ceil(brush->relative_z + s);

    result.xsize = result.xend - result.xstart + 1;
    result.zsize = result.zend - result.zstart + 1;

    return result;
}

static inline int _isInRect(IntegerRect rect, int x, int z)
{
    return (x >= rect.xstart && x <= rect.xend && z >= rect.zstart && z <= rect.zend);
}

static void _prepareBrushStroke(TerrainHeightMap* heightmap, TerrainBrush* brush)
{
    IntegerRect brush_rect = _getBrushRect(brush);

    /* Prepare floating data */
    if (heightmap->floating_used)
    {
        int gx1 = (brush_rect.xstart < heightmap->floating_data.rect.xstart) ? heightmap->floating_data.rect.xstart - brush_rect.xstart : 0;
        int gx2 = (brush_rect.xend > heightmap->floating_data.rect.xend) ? brush_rect.xend - heightmap->floating_data.rect.xend : 0;
        int gz1 = (brush_rect.zstart < heightmap->floating_data.rect.zstart) ? heightmap->floating_data.rect.zstart - brush_rect.zstart : 0;
        int gz2 = (brush_rect.zend > heightmap->floating_data.rect.zend) ? brush_rect.zend - heightmap->floating_data.rect.zend : 0;
        if (gx1 || gx2 || gz1 || gz2)
        {
            /* Floating area needs growing */
            int new_width = heightmap->floating_data.rect.xsize + gx1 + gx2;
            int new_height = heightmap->floating_data.rect.zsize + gz1 + gz2;

            heightmap->floating_data.data = memory2dRealloc(heightmap->floating_data.data, sizeof(double), heightmap->floating_data.rect.xsize, heightmap->floating_data.rect.zsize, new_width, new_height, gx1, gz1);

            heightmap->floating_data.rect.xstart -= gx1;
            heightmap->floating_data.rect.xend += gx2;
            heightmap->floating_data.rect.xsize += gx1 + gx2;
            heightmap->floating_data.rect.zstart -= gz1;
            heightmap->floating_data.rect.zend += gz2;
            heightmap->floating_data.rect.zsize += gz1 + gz2;

            _resetRect(heightmap, 0, new_width - 1, 0, gz1 - 1);
            _resetRect(heightmap, 0, new_width - 1, new_height - gz2, new_height - 1);
            _resetRect(heightmap, 0, gx1 - 1, gz1, new_height - 1 - gz2);
            _resetRect(heightmap, new_width - gx2, new_width - 1, gz1, new_height - 1 - gz2);
        }
    }
    else
    {
        /* Init floating area */
        heightmap->floating_data.rect = brush_rect;

        size_t new_size;
        new_size = sizeof(double) * brush_rect.xsize * brush_rect.zsize;
        heightmap->floating_data.data = realloc(heightmap->floating_data.data, new_size);

        _resetRect(heightmap, 0, brush_rect.xsize - 1, 0, brush_rect.zsize - 1);

        heightmap->floating_used = 1;
    }
}

size_t terrainGetMemoryStats(TerrainDefinition* definition)
{
    TerrainHeightMap* heightmap = definition->height_map;
    size_t result = 0;
    int i;

    if (heightmap->floating_used)
    {
        result += sizeof(double) * heightmap->floating_data.rect.xsize * heightmap->floating_data.rect.zsize;
    }
    for (i = 0; i < heightmap->fixed_count; i++)
    {
        result += sizeof(double) * heightmap->fixed_data[i].rect.xsize * heightmap->fixed_data[i].rect.zsize;
    }

    return result;
}

int terrainIsPainted(TerrainHeightMap* heightmap, int x, int z)
{
    int i;

    if (heightmap->floating_used && _isInRect(heightmap->floating_data.rect, x, z))
    {
        return 1;
    }
    else
    {
        for (i = 0; i < heightmap->fixed_count; i++)
        {
            if (_isInRect(heightmap->fixed_data[i].rect, x, z))
            {
                return 1;
            }
        }
        return 0;
    }
}

typedef double (*BrushCallback)(TerrainHeightMap* heightmap, TerrainBrush* brush, double x, double z, double basevalue, double influence, double force, void* data);

static inline void _applyBrush(TerrainHeightMap* heightmap, TerrainBrush* brush, double force, void* data, BrushCallback callback)
{
    IntegerRect brush_rect = _getBrushRect(brush);
    int x, z;
    double dx, dz, distance, influence;

    if (!heightmap->floating_used)
    {
        return;
    }

    for (x = brush_rect.xstart; x <= brush_rect.xend; x++)
    {
        dx = (double)x;
        for (z = brush_rect.zstart; z <= brush_rect.zend; z++)
        {
            if (_isInRect(heightmap->floating_data.rect, x, z)) /* TODO Rect intersection */
            {
                dz = (double)z;
                distance = sqrt((brush->relative_x - dx) * (brush->relative_x - dx) + (brush->relative_z - dz) * (brush->relative_z - dz));

                if (distance > brush->hard_radius)
                {
                    if (distance <= brush->hard_radius + brush->smoothed_size)
                    {
                        influence = (1.0 - (distance - brush->hard_radius) / brush->smoothed_size);
                    }
                    else
                    {
                        continue;
                    }
                }
                else
                {
                    influence = 1.0;
                }

                int ix = x - heightmap->floating_data.rect.xstart;
                int iz = z - heightmap->floating_data.rect.zstart;
                double base_value = heightmap->floating_data.data[iz * heightmap->floating_data.rect.xsize + ix];
                heightmap->floating_data.data[iz * heightmap->floating_data.rect.xsize + ix] = callback(heightmap, brush, dx, dz, base_value, influence, force, data);
            }
        }
    }
}

static double _applyBrushElevation(TerrainHeightMap* heightmap, TerrainBrush* brush, double x, double z, double basevalue, double influence, double force, void* data)
{
    UNUSED(heightmap);
    UNUSED(data);
    return basevalue + influence * force;
}

void terrainBrushElevation(TerrainHeightMap* heightmap, TerrainBrush* brush, double value)
{
    _prepareBrushStroke(heightmap, brush);
    _applyBrush(heightmap, brush, value, NULL, _applyBrushElevation);
}

static double _applyBrushSmooth(TerrainHeightMap* heightmap, TerrainBrush* brush, double x, double z, double basevalue, double influence, double force, void* data)
{
    double ideal, factor;
    ideal = terrainGetInterpolatedHeight(heightmap->terrain, x + brush->total_radius * 0.5, z, 1);
    ideal += terrainGetInterpolatedHeight(heightmap->terrain, x - brush->total_radius * 0.5, z, 1);
    ideal += terrainGetInterpolatedHeight(heightmap->terrain, x, z - brush->total_radius * 0.5, 1);
    ideal += terrainGetInterpolatedHeight(heightmap->terrain, x, z + brush->total_radius * 0.5, 1);
    ideal /= 4.0;
    factor = influence * force;
    if (factor > 1.0)
    {
        factor = 0.0;
    }
    return basevalue + (ideal - basevalue) * factor;
}

void terrainBrushSmooth(TerrainHeightMap* heightmap, TerrainBrush* brush, double value)
{
    _prepareBrushStroke(heightmap, brush);
    _applyBrush(heightmap, brush, value, NULL, _applyBrushSmooth);
}

static double _applyBrushAddNoise(TerrainHeightMap* heightmap, TerrainBrush* brush, double x, double z, double basevalue, double influence, double force, void* data)
{
    UNUSED(heightmap);
    return basevalue + noiseGet2DTotal((NoiseGenerator*)data, x / brush->total_radius, z / brush->total_radius) * influence * force * brush->total_radius;
}

void terrainBrushAddNoise(TerrainHeightMap* heightmap, TerrainBrush* brush, NoiseGenerator* generator, double value)
{
    _prepareBrushStroke(heightmap, brush);
    _applyBrush(heightmap, brush, value, generator, _applyBrushAddNoise);
}

static double _applyBrushReset(TerrainHeightMap* heightmap, TerrainBrush* brush, double x, double z, double basevalue, double influence, double force, void* data)
{
    UNUSED(brush);
    UNUSED(data);

    double ideal = terrainGetInterpolatedHeight(heightmap->terrain, x, z, 1);
    return basevalue + (ideal - basevalue) * influence * force;
}

void terrainBrushReset(TerrainHeightMap* heightmap, TerrainBrush* brush, double value)
{
    /* No need to prepare the floating data, it can't grow here */
    _applyBrush(heightmap, brush, value, NULL, _applyBrushReset);
}

void terrainEndBrushStroke(TerrainHeightMap* heightmap)
{
    /* Commit floating data to fixed */
    if (heightmap->floating_used)
    {
        /* TODO Find overlapping data and merge with them */

        size_t mapsize = sizeof(double) * heightmap->floating_data.rect.xsize * heightmap->floating_data.rect.zsize;
        heightmap->fixed_data = realloc(heightmap->fixed_data, sizeof(TerrainHeightMapChunk) * (heightmap->fixed_count + 1));
        heightmap->fixed_data[heightmap->fixed_count].rect = heightmap->floating_data.rect;
        heightmap->fixed_data[heightmap->fixed_count].data = malloc(mapsize);
        memcpy(heightmap->fixed_data[heightmap->fixed_count].data, heightmap->floating_data.data, mapsize);

        heightmap->fixed_count++;

        heightmap->floating_used = 0;
        heightmap->floating_data.data = realloc(heightmap->floating_data.data, sizeof(double));
    }
}
