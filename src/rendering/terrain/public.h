#ifndef _PAYSAGES_TERRAIN_PUBLIC_H_
#define _PAYSAGES_TERRAIN_PUBLIC_H_

#include "../rendering_global.h"
#include "../shared/types.h"
#include "../tools/euclid.h"
#include "Color.h"

typedef struct
{
    Vector3 location;
    Vector3 normal;
} TerrainResult;

typedef double (*FuncTerrainGetHeight)(Renderer* renderer, double x, double z, int with_painting);
typedef TerrainResult (*FuncTerrainGetResult)(Renderer* renderer, double x, double z, int with_painting, int with_textures);
typedef Color (*FuncTerrainGetFinalColor)(Renderer* renderer, Vector3 location, double precision);
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

RENDERINGSHARED_EXPORT extern StandardRenderer TerrainRendererClass;

RENDERINGSHARED_EXPORT void terrainAlterPreviewRenderer(Renderer* renderer);
RENDERINGSHARED_EXPORT Color terrainGetPreviewColor(Renderer* renderer, double x, double z, double detail);

#endif
