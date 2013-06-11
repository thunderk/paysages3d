#ifndef _PAYSAGES_TERRAIN_PUBLIC_H_
#define _PAYSAGES_TERRAIN_PUBLIC_H_

#include <stdlib.h>
#include "../shared/types.h"
#include "../tools/color.h"
#include "../tools/euclid.h"
#include "../noise.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    TERRAIN_PRESET_STANDARD
} TerrainPreset;

typedef struct TerrainHeightMap TerrainHeightMap;

typedef struct
{
    double height;
    double scaling;
    double shadow_smoothing;

    TerrainHeightMap* height_map;

    double water_height;

    double _detail;
    NoiseGenerator* _height_noise;
    double _min_height;
    double _max_height;
} TerrainDefinition;

typedef struct
{
    Vector3 location;
    Vector3 normal;
} TerrainResult;

typedef double (*FuncTerrainGetHeight)(Renderer* renderer, double x, double z, int with_painting);
typedef TerrainResult (*FuncTerrainGetResult)(Renderer* renderer, double x, double z, int with_painting, int with_textures);
typedef Color(*FuncTerrainGetFinalColor)(Renderer* renderer, Vector3 location, double precision);
typedef double (*FuncGetWaterHeight)(Renderer* renderer);

typedef struct
{
    TerrainDefinition* definition;

    FuncGeneralCastRay castRay;
    FuncTerrainGetHeight getHeight;
    FuncTerrainGetResult getResult;
    FuncTerrainGetFinalColor getFinalColor;
    FuncGetWaterHeight getWaterHeight;

    void* _internal_data;
} TerrainRenderer;

extern StandardDefinition TerrainDefinitionClass;
extern StandardRenderer TerrainRendererClass;

void terrainAutoPreset(TerrainDefinition* definition, TerrainPreset preset);
void terrainRenderSurface(Renderer* renderer);
double terrainGetGridHeight(TerrainDefinition* definition, int x, int z, int with_painting);
double terrainGetInterpolatedHeight(TerrainDefinition* definition, double x, double z, int with_painting);
size_t terrainGetMemoryStats(TerrainDefinition* definition);

Renderer* terrainCreatePreviewRenderer();
Color terrainGetPreviewColor(Renderer* renderer, double x, double z, double detail);

typedef struct
{
    double relative_x;
    double relative_z;
    double hard_radius;
    double smoothed_size;
    double total_radius;
} TerrainBrush;

/* Heightmap manipulation */
int terrainIsPainted(TerrainHeightMap* heightmap, int x, int z);
void terrainClearPainting(TerrainHeightMap* heightmap);
void terrainBrushElevation(TerrainHeightMap* heightmap, TerrainBrush* brush, double value);
void terrainBrushSmooth(TerrainHeightMap* heightmap, TerrainBrush* brush, double value);
void terrainBrushAddNoise(TerrainHeightMap* heightmap, TerrainBrush* brush, NoiseGenerator* generator, double value);
void terrainBrushReset(TerrainHeightMap* heightmap, TerrainBrush* brush, double value);
void terrainBrushFlatten(TerrainHeightMap* heightmap, TerrainBrush* brush, double height, double force);
void terrainEndBrushStroke(TerrainHeightMap* heightmap);

#ifdef __cplusplus
}
#endif

#endif
