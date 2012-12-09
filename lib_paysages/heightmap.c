#include "heightmap.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "tools.h"
#include "system.h"
#include "noise.h"

HeightMap heightmapCreate()
{
    HeightMap result;

    result.data = malloc(sizeof(double));
    result.resolution_x = 1;
    result.resolution_z = 1;

    return result;
}

void heightmapDelete(HeightMap* heightmap)
{
    free(heightmap->data);
}

void heightmapCopy(HeightMap* source, HeightMap* destination)
{
    destination->resolution_x = source->resolution_x;
    destination->resolution_z = source->resolution_z;
    destination->data = realloc(destination->data, sizeof(double) * destination->resolution_x * destination->resolution_z);
    memcpy(destination->data, source->data, sizeof(double) * destination->resolution_x * destination->resolution_z);
}

void heightmapValidate(HeightMap* heightmap)
{
    UNUSED(heightmap);
}

void heightmapSave(PackStream* stream, HeightMap* heightmap)
{
    int i;

    packWriteInt(stream, &heightmap->resolution_x);
    packWriteInt(stream, &heightmap->resolution_z);
    for (i = 0; i < heightmap->resolution_x * heightmap->resolution_z; i++)
    {
        packWriteDouble(stream, &heightmap->data[i]);
    }
}

void heightmapLoad(PackStream* stream, HeightMap* heightmap)
{
    int i;

    packReadInt(stream, &heightmap->resolution_x);
    packReadInt(stream, &heightmap->resolution_z);
    heightmap->data = realloc(heightmap->data, sizeof(double) * heightmap->resolution_x * heightmap->resolution_z);
    for (i = 0; i < heightmap->resolution_x * heightmap->resolution_z; i++)
    {
        packReadDouble(stream, &heightmap->data[i]);
    }
}

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


static inline void _getBrushBoundaries(HeightMapBrush* brush, int rx, int rz, int* x1, int* x2, int* z1, int* z2)
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

typedef double (*BrushCallback)(HeightMap* heightmap, HeightMapBrush* brush, double x, double z, double basevalue, double influence, double force, void* data);

static inline void _applyBrush(HeightMap* heightmap, HeightMapBrush* brush, double force, void* data, BrushCallback callback)
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

static double _applyBrushElevation(HeightMap* heightmap, HeightMapBrush* brush, double x, double z, double basevalue, double influence, double force, void* data)
{
    return basevalue + influence * force * brush->total_radius;
}

void heightmapBrushElevation(HeightMap* heightmap, HeightMapBrush* brush, double value)
{
    _applyBrush(heightmap, brush, value, NULL, _applyBrushElevation);
}

static double _applyBrushSmooth(HeightMap* heightmap, HeightMapBrush* brush, double x, double z, double basevalue, double influence, double force, void* data)
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

void heightmapBrushSmooth(HeightMap* heightmap, HeightMapBrush* brush, double value)
{
    _applyBrush(heightmap, brush, value, NULL, _applyBrushSmooth);
}

static double _applyBrushAddNoise(HeightMap* heightmap, HeightMapBrush* brush, double x, double z, double basevalue, double influence, double force, void* data)
{
    return basevalue + noiseGet2DTotal((NoiseGenerator*)data, x / brush->total_radius, z / brush->total_radius) * influence * force * brush->total_radius;
}

void heightmapBrushAddNoise(HeightMap* heightmap, HeightMapBrush* brush, NoiseGenerator* generator, double value)
{
    _applyBrush(heightmap, brush, value, generator, _applyBrushAddNoise);
}
