#ifndef _PAYSAGES_WATER_PUBLIC_H_
#define _PAYSAGES_WATER_PUBLIC_H_

#include "../shared/types.h"
#include "../tools/lighting.h"
#include "../tools/curve.h"
#include "../tools/euclid.h"
#include "../noise.h"
#include "../layers.h"

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
    double height;
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


extern StandardDefinition WaterDefinitionClass;
extern StandardRenderer WaterRendererClass;

void waterRenderSurface(Renderer* renderer);
void waterAutoPreset(WaterDefinition* definition, WaterPreset preset);

Renderer* waterCreatePreviewCoverageRenderer();
Color waterGetPreviewCoverage(Renderer* renderer, double x, double y, double scaling, int highlight_enabled);

Renderer* waterCreatePreviewColorRenderer();
Color waterGetPreviewColor(Renderer* renderer, double x, double y, double scaling);

#ifdef __cplusplus
}
#endif

#endif
