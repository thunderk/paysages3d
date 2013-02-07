#ifndef _PAYSAGES_TERRAIN_PRIVATE_H_
#define _PAYSAGES_TERRAIN_PRIVATE_H_

#include "public.h"

typedef struct
{
    int xstart;
    int xend;
    int xsize;
    int zstart;
    int zend;
    int zsize;
} IntegerRect;

typedef struct
{
    IntegerRect rect;
    double* data;
} TerrainHeightMapChunk;

struct TerrainHeightMap
{
    TerrainDefinition* terrain;
    int fixed_count;
    TerrainHeightMapChunk* fixed_data;
    int floating_used;
    TerrainHeightMapChunk floating_data;
};

TerrainHeightMap* terrainHeightMapCreate(TerrainDefinition* definition);
void terrainHeightmapDelete(TerrainHeightMap* heightmap);
void terrainHeightmapCopy(TerrainHeightMap* source, TerrainHeightMap* destination);
void terrainHeightmapSave(PackStream* stream, TerrainHeightMap* heightmap);
void terrainHeightmapLoad(PackStream* stream, TerrainHeightMap* heightmap);
int terrainHeightmapGetHeight(TerrainHeightMap* heightmap, double x, double z, double* result);

#endif
