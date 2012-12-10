#ifndef _PAYSAGES_TERRAIN_PRIVATE_H_
#define _PAYSAGES_TERRAIN_PRIVATE_H_

#include "public.h"

#define TERRAIN_HEIGHTMAP_DETAIL 0.1

typedef struct
{
    int xstart;
    int ystart;
    int xsize;
    int ysize;
    void* data;
} TerrainHeightMapData;

typedef struct
{
    int fixed_count;
    TerrainHeightMapData* fixed_data;
    int floating_count;
    TerrainHeightMapData* floating_data;
} TerrainHeightMap;

#endif
