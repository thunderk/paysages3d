#include "private.h"

/*
 * Terrain height map painting.
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../tools/memory.h"

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
        TerrainHeightMapChunk* chunk = destination->fixed_data + i;

        chunk->rect = chunk->rect;
        if (chunk->rect.xsize * chunk->rect.zsize > 0)
        {
            chunk->data = realloc(chunk->data, sizeof(double) * chunk->rect.xsize * chunk->rect.zsize);
        }
        memcpy(chunk->data, chunk->data, sizeof(double) * chunk->rect.xsize * chunk->rect.zsize);
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
    if (x > (double)chunk->rect.xstart && x < (double)chunk->rect.xend && z > (double)chunk->rect.zstart && z < (double)chunk->rect.zend)
    {
        /* TODO Get interpolated value */
        *result = 0.0;
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

    for (i = 0; i < heightmap->fixed_count; i++)
    {
        if (_checkDataHit(heightmap->fixed_data + i, x, z, result))
        {
            return 1;
        }
    }

    if (heightmap->floating_used && _checkDataHit(&heightmap->floating_data, x, z, result))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static inline void _resetRect(TerrainHeightMap* heightmap, int x1, int x2, int z1, int z2)
{
    int x, z;

    for (x = x1; x <= x2; x++)
    {
        for (z = z1; z <= z2; z++)
        {
            heightmap->floating_data.data[z * heightmap->floating_data.rect.xsize + x] = terrainGetGridHeight(heightmap->terrain, x + heightmap->floating_data.rect.xstart, z + heightmap->floating_data.rect.zstart, 1);
        }
    }
}

static void _prepareBrushStroke(TerrainHeightMap* heightmap, TerrainBrush* brush)
{
    double s = brush->smoothed_size + brush->hard_radius;
    int x1 = (int)floor(brush->relative_x - s);
    int x2 = (int)ceil(brush->relative_x + s);
    int z1 = (int)floor(brush->relative_z - s);
    int z2 = (int)ceil(brush->relative_z + s);

    /* Prepare floating data */
    if (heightmap->floating_used)
    {
        int gx1 = (x1 < heightmap->floating_data.rect.xstart) ? heightmap->floating_data.rect.xstart - x1 : 0;
        int gx2 = (x2 > heightmap->floating_data.rect.xend) ? x2 - heightmap->floating_data.rect.xend : 0;
        int gz1 = (z1 < heightmap->floating_data.rect.zstart) ? heightmap->floating_data.rect.zstart - z1 : 0;
        int gz2 = (z2 > heightmap->floating_data.rect.zend) ? z2 - heightmap->floating_data.rect.zend : 0;
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
            _resetRect(heightmap, 0, new_width - 1, new_height - gz2 + 1, new_height - 1);
            _resetRect(heightmap, 0, gx1 - 1, gz1, new_height - gz2);
            _resetRect(heightmap, new_width - gx2 + 1, new_width - 1, gz1, new_height - gz2);
        }
    }
    else
    {
        /* Init floating area */
        heightmap->floating_data.rect.xstart = x1;
        heightmap->floating_data.rect.xend = x2;
        heightmap->floating_data.rect.xsize = x2 - x1 + 1;
        heightmap->floating_data.rect.zstart = z1;
        heightmap->floating_data.rect.zend = z2;
        heightmap->floating_data.rect.zsize = z2 - z1 + 1;

        size_t new_size;
        new_size = sizeof(double) * heightmap->floating_data.rect.xsize * heightmap->floating_data.rect.zsize;
        heightmap->floating_data.data = realloc(heightmap->floating_data.data, new_size);

        _resetRect(heightmap, 0, 0, heightmap->floating_data.rect.xsize - 1, heightmap->floating_data.rect.zsize - 1);

        heightmap->floating_used = 1;
    }
}

void terrainBrushElevation(TerrainHeightMap* heightmap, TerrainBrush* brush, double value)
{
    _prepareBrushStroke(heightmap, brush);
}

void terrainBrushSmooth(TerrainHeightMap* heightmap, TerrainBrush* brush, double value)
{
    _prepareBrushStroke(heightmap, brush);
}

void terrainBrushAddNoise(TerrainHeightMap* heightmap, TerrainBrush* brush, NoiseGenerator* generator, double value)
{
    _prepareBrushStroke(heightmap, brush);
}

void terrainBrushReset(TerrainHeightMap* heightmap, TerrainBrush* brush, double value)
{
}

#if 0
static void _loadFromFilePixel(HeightMap* heightmap, int x, int y, Color col)
{
    assert(x >= 0 && x < heightmap->resolution_x);
    assert(y >= 0 && y < heightmap->resolution_z);

    heightmap->data[y * heightmap->resolution_x + x] = (col.r + col.g + col.b) / 3.0;
}

void heightmapGetLimits(HeightMap* heightmap, double* ymin, double* ymax)
{
    double y;
    int i;
    *ymin = 1000000.0;
    *ymax = -1000000.0;
    /* TODO Keep the value in cache */
    for (i = 0; i < heightmap->resolution_x * heightmap->resolution_z; i++)
    {
        y = heightmap->data[i];
        if (y < *ymin)
        {
            *ymin = y;
        }
        if (y > *ymax)
        {
            *ymax = y;
        }
    }
}

double heightmapGetRawValue(HeightMap* heightmap, double x, double z)
{
    assert(x >= 0.0 && x <= 1.0);
    assert(z >= 0.0 && z <= 1.0);

    return heightmap->data[((int)(z * (double)(heightmap->resolution_z - 1))) * heightmap->resolution_x + ((int)(x * (double)(heightmap->resolution_x - 1)))];
}

double heightmapGetValue(HeightMap* heightmap, double x, double z)
{
    int xmax = heightmap->resolution_x - 1;
    int zmax = heightmap->resolution_z - 1;
    int xlow;
    int zlow;
    double stencil[16];
    int ix, iz, cx, cz;

    if (x < 0.0)
    {
        x = 0.0;
    }
    else if (x > 1.0)
    {
        x = 1.0;
    }
    if (z < 0.0)
    {
        z = 0.0;
    }
    else if (z > 1.0)
    {
        z = 1.0;
    }

    xlow = floor(x * xmax);
    zlow = floor(z * zmax);

    for (ix = xlow - 1; ix <= xlow + 2; ix++)
    {
        for (iz = zlow - 1; iz <= zlow + 2; iz++)
        {
            cx = ix < 0 ? 0 : ix;
            cx = cx > xmax ? xmax : cx;
            cz = iz < 0 ? 0 : iz;
            cz = cz > zmax ? zmax : cz;
            stencil[(iz - (zlow - 1)) * 4 + ix - (xlow - 1)] = heightmap->data[cz * heightmap->resolution_x + cx];
        }
    }

    return toolsBicubicInterpolate(stencil, x * xmax - (double)xlow, z * zmax - (double)zlow);
}


void heightmapImportFromPicture(HeightMap* heightmap, const char* picturepath)
{
    systemLoadPictureFile(picturepath, (PictureCallbackLoadStarted)heightmapChangeResolution, (PictureCallbackLoadPixel)_loadFromFilePixel, heightmap);
}

void heightmapChangeResolution(HeightMap* heightmap, int resolution_x, int resolution_z)
{
    int i;

    heightmap->resolution_x = resolution_x;
    heightmap->resolution_z = resolution_z;
    heightmap->data = realloc(heightmap->data, sizeof(double) * heightmap->resolution_x * heightmap->resolution_z);

    for (i = 0; i < heightmap->resolution_x * heightmap->resolution_z; i++)
    {
        heightmap->data[i] = 0.0;
    }
}

void heightmapRevertToTerrain(HeightMap* heightmap, TerrainDefinition* terrain, GeoArea* area)
{
    int rx, rz;
    int x, z;
    double dx, dz;

    rx = heightmap->resolution_x;
    rz = heightmap->resolution_z;
    for (x = 0; x < rx; x++)
    {
        for (z = 0; z < rz; z++)
        {
            dx = (double)x / (double)(rx - 1);
            dz = (double)z / (double)(rz - 1);
            geoareaFromLocal(area, dx, dz, &dx, &dz);
//            heightmap->data[z * rx + x] = terrainGetHeight(terrain, dx, dz);
        }
    }
}


static inline void _getBrushBoundaries(TerrainBrush* brush, int rx, int rz, int* x1, int* x2, int* z1, int* z2)
{
    double cx = brush->relative_x * rx;
    double cz = brush->relative_z * rz;
    double s = brush->smoothed_size + brush->hard_radius;
    double sx = s * rx;
    double sz = s * rz;
    *x1 = (int)floor(cx - sx);
    *x2 = (int)ceil(cx + sx);
    *z1 = (int)floor(cz - sz);
    *z2 = (int)ceil(cz + sz);
    if (*x1 < 0)
    {
        *x1 = 0;
    }
    else if (*x1 > rx)
    {
        *x1 = rx;
    }
    if (*x2 < 0)
    {
        *x2 = 0;
    }
    else if (*x2 > rx)
    {
        *x2 = rx;
    }
    if (*z1 < 0)
    {
        *z1 = 0;
    }
    else if (*z1 > rz)
    {
        *z1 = rz;
    }
    if (*z2 < 0)
    {
        *z2 = 0;
    }
    else if (*z2 > rz)
    {
        *z2 = rz;
    }
}

typedef double (*BrushCallback)(HeightMap* heightmap, TerrainBrush* brush, double x, double z, double basevalue, double influence, double force, void* data);

static inline void _applyBrush(HeightMap* heightmap, TerrainBrush* brush, double force, void* data, BrushCallback callback)
{
    int x, x1, x2, z, z1, z2;
    double dx, dz, distance, influence;

    _getBrushBoundaries(brush, heightmap->resolution_x - 1, heightmap->resolution_z - 1, &x1, &x2, &z1, &z2);

    for (x = x1; x <= x2; x++)
    {
        dx = (double)x / (double)heightmap->resolution_x;
        for (z = z1; z <= z2; z++)
        {
            dz = (double)z / (double)heightmap->resolution_z;
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

            heightmap->data[z * heightmap->resolution_x + x] = callback(heightmap, brush, dx, dz, heightmap->data[z * heightmap->resolution_x + x], influence, force, data);
        }
    }
}

static double _applyBrushElevation(HeightMap* heightmap, TerrainBrush* brush, double x, double z, double basevalue, double influence, double force, void* data)
{
    return basevalue + influence * force * brush->total_radius;
}

void terrainBrushElevation(HeightMap* heightmap, TerrainBrush* brush, double value)
{
    _applyBrush(heightmap, brush, value, NULL, _applyBrushElevation);
}

static double _applyBrushSmooth(HeightMap* heightmap, TerrainBrush* brush, double x, double z, double basevalue, double influence, double force, void* data)
{
    double ideal, factor;
    ideal = heightmapGetValue(heightmap, x + brush->total_radius * 0.5, z);
    ideal += heightmapGetValue(heightmap, x - brush->total_radius * 0.5, z);
    ideal += heightmapGetValue(heightmap, x, z - brush->total_radius * 0.5);
    ideal += heightmapGetValue(heightmap, x, z + brush->total_radius * 0.5);
    ideal /= 4.0;
    factor = influence * force;
    if (factor > 1.0)
    {
        factor = 0.0;
    }
    return basevalue + (ideal - basevalue) * factor;
}

void terrainBrushSmooth(HeightMap* heightmap, TerrainBrush* brush, double value)
{
    _applyBrush(heightmap, brush, value, NULL, _applyBrushSmooth);
}

static double _applyBrushAddNoise(HeightMap* heightmap, TerrainBrush* brush, double x, double z, double basevalue, double influence, double force, void* data)
{
    return basevalue + noiseGet2DTotal((NoiseGenerator*)data, x / brush->total_radius, z / brush->total_radius) * influence * force * brush->total_radius;
}

void terrainBrushAddNoise(HeightMap* heightmap, TerrainBrush* brush, NoiseGenerator* generator, double value)
{
    _applyBrush(heightmap, brush, value, generator, _applyBrushAddNoise);
}
#endif
