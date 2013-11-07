#ifndef _PAYSAGES_TERRAIN_PUBLIC_H_
#define _PAYSAGES_TERRAIN_PUBLIC_H_

#include "../rendering_global.h"
#include <stdlib.h>
#include "../shared/types.h"
#include "../tools/color.h"
#include "../tools/euclid.h"

namespace paysages {
namespace basics {
    class NoiseGenerator;
}
}

typedef enum
{
    TERRAIN_PRESET_STANDARD
} TerrainPreset;

typedef struct TerrainHeightMap TerrainHeightMap;

class TerrainDefinition
{
public:
    double height;
    double scaling;
    double shadow_smoothing;

    TerrainHeightMap* height_map;

    double water_height;

    double _detail;
    NoiseGenerator* _height_noise;
    double _min_height;
    double _max_height;
};

typedef struct
{
    Vector3 location;
    Vector3 normal;
} TerrainResult;

typedef double (*FuncTerrainGetHeight)(Renderer* renderer, double x, double z, int with_painting);
typedef TerrainResult (*FuncTerrainGetResult)(Renderer* renderer, double x, double z, int with_painting, int with_textures);
typedef Color(*FuncTerrainGetFinalColor)(Renderer* renderer, Vector3 location, double precision);
typedef double (*FuncGetWaterHeight)(Renderer* renderer);

class TerrainRenderer
{
public:
    TerrainDefinition* definition;

    FuncGeneralCastRay castRay;
    FuncTerrainGetHeight getHeight;
    FuncTerrainGetResult getResult;
    FuncTerrainGetFinalColor getFinalColor;
    FuncGetWaterHeight getWaterHeight;

    void* _internal_data;
};

RENDERINGSHARED_EXPORT extern StandardDefinition TerrainDefinitionClass;
RENDERINGSHARED_EXPORT extern StandardRenderer TerrainRendererClass;

RENDERINGSHARED_EXPORT void terrainAutoPreset(TerrainDefinition* definition, TerrainPreset preset);
RENDERINGSHARED_EXPORT double terrainGetGridHeight(TerrainDefinition* definition, int x, int z, int with_painting);
RENDERINGSHARED_EXPORT double terrainGetInterpolatedHeight(TerrainDefinition* definition, double x, double z, int scaled, int with_painting);
RENDERINGSHARED_EXPORT size_t terrainGetMemoryStats(TerrainDefinition* definition);

RENDERINGSHARED_EXPORT Renderer* terrainCreatePreviewRenderer();
RENDERINGSHARED_EXPORT Color terrainGetPreviewColor(Renderer* renderer, double x, double z, double detail);

RENDERINGSHARED_EXPORT HeightInfo terrainGetHeightInfo(TerrainDefinition* definition);

typedef struct
{
    double relative_x;
    double relative_z;
    double hard_radius;
    double smoothed_size;
    double total_radius;
} TerrainBrush;

/* Heightmap manipulation */
RENDERINGSHARED_EXPORT int terrainIsPainted(TerrainHeightMap* heightmap, int x, int z);
RENDERINGSHARED_EXPORT void terrainClearPainting(TerrainHeightMap* heightmap);
RENDERINGSHARED_EXPORT void terrainBrushElevation(TerrainHeightMap* heightmap, TerrainBrush* brush, double value);
RENDERINGSHARED_EXPORT void terrainBrushSmooth(TerrainHeightMap* heightmap, TerrainBrush* brush, double value);
RENDERINGSHARED_EXPORT void terrainBrushAddNoise(TerrainHeightMap* heightmap, TerrainBrush* brush, NoiseGenerator* generator, double value);
RENDERINGSHARED_EXPORT void terrainBrushReset(TerrainHeightMap* heightmap, TerrainBrush* brush, double value);
RENDERINGSHARED_EXPORT void terrainBrushFlatten(TerrainHeightMap* heightmap, TerrainBrush* brush, double height, double force);
RENDERINGSHARED_EXPORT void terrainEndBrushStroke(TerrainHeightMap* heightmap);

#endif
