#ifndef _PAYSAGES_TERRAIN_PUBLIC_H_
#define _PAYSAGES_TERRAIN_PUBLIC_H_

#include "../shared/types.h"
#include "../noise.h"
#include "../lighting.h"

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

    double _detail;
    NoiseGenerator* _height_noise;
    double _min_height;
    double _max_height;
} TerrainDefinition;

typedef double (*FuncTerrainGetHeight)(Renderer* renderer, double x, double z, int with_painting);
typedef Color (*FuncTerrainGetFinalColor)(Renderer* renderer, Vector3 location, double precision);

typedef struct
{
    TerrainDefinition* definition;

    FuncGeneralCastRay castRay;
    FuncLightingAlterLight alterLight;
    FuncTerrainGetHeight getHeight;
    FuncTerrainGetFinalColor getFinalColor;

    void* _internal_data;
} TerrainRenderer;

extern StandardDefinition TerrainDefinitionClass;
extern StandardRenderer TerrainRendererClass;

void terrainAutoPreset(TerrainDefinition* definition, TerrainPreset preset);
void terrainRenderSurface(Renderer* renderer);
double terrainGetGridHeight(TerrainDefinition* definition, int x, int z, int with_painting);

Renderer terrainCreatePreviewRenderer();
Color terrainGetPreviewColor(Renderer* renderer, double x, double z, double detail);

typedef struct
{
    double relative_x;
    double relative_z;
    double hard_radius;
    double smoothed_size;
    double total_radius;
} TerrainBrush;

void terrainBrushElevation(TerrainHeightMap* heightmap, TerrainBrush* brush, double value);
void terrainBrushSmooth(TerrainHeightMap* heightmap, TerrainBrush* brush, double value);
void terrainBrushAddNoise(TerrainHeightMap* heightmap, TerrainBrush* brush, NoiseGenerator* generator, double value);
void terrainBrushReset(TerrainHeightMap* heightmap, TerrainBrush* brush, double value);

#ifdef __cplusplus
}
#endif

#endif
