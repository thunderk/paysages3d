#ifndef _PAYSAGES_TERRAIN_PRIVATE_H_
#define _PAYSAGES_TERRAIN_PRIVATE_H_

#include "public.h"

typedef struct
{
    int xstart;
    int zstart;
    int xsize;
    int zsize;
    double* data;
} TerrainHeightMapData;

struct TerrainHeightMap
{
    int fixed_count;
    TerrainHeightMapData* fixed_data;
    int floating_used;
    TerrainHeightMapData floating_data;
};

TerrainHeightMap* terrainHeightMapCreate();
void terrainHeightmapDelete(TerrainHeightMap* heightmap);
void terrainHeightmapCopy(TerrainHeightMap* source, TerrainHeightMap* destination);
void terrainHeightmapSave(PackStream* stream, TerrainHeightMap* heightmap);
void terrainHeightmapLoad(PackStream* stream, TerrainHeightMap* heightmap);
int terrainHeightmapGetHeight(TerrainHeightMap* heightmap, double x, double z, double* result);

#endif
