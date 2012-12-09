#ifndef _PAYSAGES_HEIGHTMAP_H_
#define _PAYSAGES_HEIGHTMAP_H_

/* Height map for terrain */

#include "pack.h"
#include "noise.h"
#include "geoarea.h"
#include "terrain/public.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    int resolution_x;
    int resolution_z;
    double* data;
} HeightMap;

typedef struct
{
    double relative_x;
    double relative_z;
    double hard_radius;
    double smoothed_size;
    double total_radius;
} HeightMapBrush;

HeightMap heightmapCreate();
void heightmapDelete(HeightMap* heightmap);
void heightmapCopy(HeightMap* source, HeightMap* destination);
void heightmapValidate(HeightMap* heightmap);

void heightmapSave(PackStream* stream, HeightMap* heightmap);
void heightmapLoad(PackStream* stream, HeightMap* heightmap);

void heightmapGetLimits(HeightMap* heightmap, double* ymin, double* ymax);
double heightmapGetRawValue(HeightMap* heightmap, double x, double z);
double heightmapGetValue(HeightMap* heightmap, double x, double z);

void heightmapChangeResolution(HeightMap* heightmap, int resolution_x, int resolution_z);
void heightmapImportFromPicture(HeightMap* heightmap, const char* picturepath);
void heightmapRevertToTerrain(HeightMap* heightmap, TerrainDefinition* terrain, GeoArea* area);

void heightmapBrushElevation(HeightMap* heightmap, HeightMapBrush* brush, double value);
void heightmapBrushSmooth(HeightMap* heightmap, HeightMapBrush* brush, double value);
void heightmapBrushAddNoise(HeightMap* heightmap, HeightMapBrush* brush, NoiseGenerator* generator, double value);

#ifdef __cplusplus
}
#endif

#endif
