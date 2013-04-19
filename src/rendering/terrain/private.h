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

TerrainHeightMap* terrainHeightMapCreate(TerrainDefinition* definition);
void terrainHeightmapDelete(TerrainHeightMap* heightmap);
void terrainHeightmapCopy(TerrainHeightMap* source, TerrainHeightMap* destination);
void terrainHeightmapSave(PackStream* stream, TerrainHeightMap* heightmap);
void terrainHeightmapLoad(PackStream* stream, TerrainHeightMap* heightmap);
int terrainHeightmapGetInterpolatedHeight(TerrainHeightMap* heightmap, double x, double z, double* result);
int terrainHeightmapGetGridHeight(TerrainHeightMap* heightmap, int x, int z, double* result);

#endif
