#ifndef _PAYSAGES_WATER_PUBLIC_H_
#define _PAYSAGES_WATER_PUBLIC_H_

#include "../rendering_global.h"
#include "../shared/types.h"
#include "../tools/lighting.h"
#include "../tools/curve.h"
#include "../tools/euclid.h"
#include "../noise.h"
#include "Layers.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    WATER_PRESET_LAKE,
    WATER_PRESET_SEA
} WaterPreset;

typedef struct
{
    double transparency;
    double reflection;
    SurfaceMaterial material;
    Color depth_color;
    double transparency_depth;
    double lighting_depth;

    double scaling;
    double turbulence;
    double waves_height;
    double detail_height;

    double foam_coverage;
    SurfaceMaterial foam_material;

    NoiseGenerator* _waves_noise;
} WaterDefinition;

typedef struct
{
    Vector3 location;
    Color base;
    Color reflected;
    Color refracted;
    Color foam;
    Color final;
} WaterResult;

typedef HeightInfo (*FuncWaterGetHeightInfo)(Renderer* renderer);
typedef double (*FuncWaterGetHeight)(Renderer* renderer, double x, double z);
typedef WaterResult (*FuncWaterGetResult)(Renderer* renderer, double x, double z);

typedef struct
{
    WaterDefinition* definition;

    FuncWaterGetHeightInfo getHeightInfo;
    FuncWaterGetHeight getHeight;
    FuncWaterGetResult getResult;
} WaterRenderer;


RENDERINGSHARED_EXPORT extern StandardDefinition WaterDefinitionClass;
RENDERINGSHARED_EXPORT extern StandardRenderer WaterRendererClass;

RENDERINGSHARED_EXPORT void waterRenderSurface(Renderer* renderer);
RENDERINGSHARED_EXPORT void waterAutoPreset(WaterDefinition* definition, WaterPreset preset);

RENDERINGSHARED_EXPORT Renderer* waterCreatePreviewCoverageRenderer();
RENDERINGSHARED_EXPORT Color waterGetPreviewCoverage(Renderer* renderer, double x, double y, double scaling, int highlight_enabled);

RENDERINGSHARED_EXPORT Renderer* waterCreatePreviewColorRenderer();
RENDERINGSHARED_EXPORT Color waterGetPreviewColor(Renderer* renderer, double x, double y, double scaling);

#ifdef __cplusplus
}
#endif

#endif
