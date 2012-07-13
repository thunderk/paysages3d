#ifndef _PAYSAGES_HEIGHTMAP_H_
#define _PAYSAGES_HEIGHTMAP_H_

/* Height map for terrain */

#include "pack.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    int resolution_x;
    int resolution_z;
    double* data;
} HeightMap;

HeightMap heightmapCreate();
void heightmapDelete(HeightMap* heightmap);
void heightmapCopy(HeightMap* source, HeightMap* destination);
void heightmapValidate(HeightMap* heightmap);

void heightmapSave(PackStream* stream, HeightMap* heightmap);
void heightmapLoad(PackStream* stream, HeightMap* heightmap);

void heightmapChangeResolution(HeightMap* heightmap, int resolution_x, int resolution_z);
    
#ifdef __cplusplus
}
#endif

#endif
