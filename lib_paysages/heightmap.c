#include "heightmap.h"

#include <stdlib.h>
#include <string.h>

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
    for (i = 0; i < heightmap->resolution_x * heightmap->resolution_z; i++)
    {
        packReadDouble(stream, &heightmap->data[i]);
    }
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
