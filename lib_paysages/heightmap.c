#include "heightmap.h"
#include "tools.h"
#include "system.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

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

double heightmapGetValue(HeightMap* heightmap, double x, double z)
{
    int xmax = heightmap->resolution_x - 1;
    int zmax = heightmap->resolution_z - 1;
    int xlow = floor(x * xmax);
    int zlow = floor(z * zmax);
    double stencil[16];
    int ix, iz, cx, cz;
    
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

void heightmapBrushElevation(HeightMap* heightmap, HeightMapBrush* brush, double value)
{
    int x, z;
    double dx, dz, distance;
    
    /* TODO Limit to brush radius */
    
    for (x = 0; x < heightmap->resolution_x; x++)
    {
        dx = (double)x / (double)heightmap->resolution_x;
        for (z = 0; z < heightmap->resolution_z; z++)
        {
            dz = (double)z / (double)heightmap->resolution_z;
            distance = sqrt((brush->relative_x - dx) * (brush->relative_x - dx) + (brush->relative_z - dz) * (brush->relative_z - dz));
            
            if (distance > brush->hard_radius)
            {
                if (distance <= brush->hard_radius + brush->smoothed_size)
                {
                    heightmap->data[z * heightmap->resolution_x +x] += value * (1.0 - (distance - brush->hard_radius) / brush->smoothed_size);
                }
            }
            else
            {
                heightmap->data[z * heightmap->resolution_x +x] += value;
            }
        }
    }
}
