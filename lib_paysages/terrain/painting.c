#include "private.h"

/*
 * Terrain height map painting.
 */

#include <stdlib.h>
#include <string.h>

TerrainHeightMap* terrainHeightMapCreate()
{
    TerrainHeightMap* result;

    result = malloc(sizeof(TerrainHeightMap));
    result->fixed_count = 0;
    result->fixed_data = malloc(sizeof(TerrainHeightMapData));
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
        heightmap->fixed_data = realloc(heightmap->fixed_data, sizeof(TerrainHeightMapData) * new_count);
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
            heightmap->fixed_data = realloc(heightmap->fixed_data, sizeof(TerrainHeightMapData) * new_count);
        }
    }

    heightmap->fixed_count = new_count;
}

void terrainHeightmapCopy(TerrainHeightMap* source, TerrainHeightMap* destination)
{
    int i, j;

    _setFixedCount(destination, source->fixed_count);

    for (i = 0; i < destination->fixed_count; i++)
    {
        destination->fixed_data[i].xstart = source->fixed_data[i].xstart;
        destination->fixed_data[i].xsize = source->fixed_data[i].xsize;
        destination->fixed_data[i].ystart = source->fixed_data[i].ystart;
        destination->fixed_data[i].ysize = source->fixed_data[i].ysize;
        if (destination->fixed_data[i].xsize * destination->fixed_data[i].ysize > 0)
        {
            destination->fixed_data[i].data = realloc(destination->fixed_data[i].data, sizeof(double) * destination->fixed_data[i].xsize * destination->fixed_data[i].ysize);
        }
        memcpy(destination->fixed_data[i].data, source->fixed_data[i].data, sizeof(double) * destination->fixed_data[i].xsize * destination->fixed_data[i].ysize);
    }

    destination->floating_used = 0;
}

void terrainHeightmapSave(PackStream* stream, TerrainHeightMap* heightmap)
{
    int i, j;

    packWriteInt(stream, &heightmap->fixed_count);
    for (i = 0; i < heightmap->fixed_count; i++)
    {
        packWriteInt(stream, &heightmap->fixed_data[i].xstart);
        packWriteInt(stream, &heightmap->fixed_data[i].xsize);
        packWriteInt(stream, &heightmap->fixed_data[i].ystart);
        packWriteInt(stream, &heightmap->fixed_data[i].ysize);
        for (j = 0; j < heightmap->fixed_data[i].xsize * heightmap->fixed_data[i].ysize; j++)
        {
            packWriteDouble(stream, &heightmap->fixed_data[i].data[j]);
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
        packReadInt(stream, &heightmap->fixed_data[i].xstart);
        packReadInt(stream, &heightmap->fixed_data[i].xsize);
        packReadInt(stream, &heightmap->fixed_data[i].ystart);
        packReadInt(stream, &heightmap->fixed_data[i].ysize);
        if (heightmap->fixed_data[i].xsize * heightmap->fixed_data[i].ysize > 0)
        {
            heightmap->fixed_data[i].data = realloc(heightmap->fixed_data[i].data, sizeof(double) * heightmap->fixed_data[i].xsize * heightmap->fixed_data[i].ysize);
        }
        for (j = 0; j < heightmap->fixed_data[i].xsize * heightmap->fixed_data[i].ysize; j++)
        {
            packReadDouble(stream, &heightmap->fixed_data[i].data[j]);
        }
    }

    heightmap->floating_used = 0;
}

void terrainBrushElevation(TerrainHeightMap* heightmap, TerrainBrush* brush, double value)
{
}

void terrainBrushSmooth(TerrainHeightMap* heightmap, TerrainBrush* brush, double value)
{
}

void terrainBrushAddNoise(TerrainHeightMap* heightmap, TerrainBrush* brush, NoiseGenerator* generator, double value)
{
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
