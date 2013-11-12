#ifndef _PAYSAGES_WATER_PUBLIC_H_
#define _PAYSAGES_WATER_PUBLIC_H_

#include "../rendering_global.h"
#include "../shared/types.h"
#include "../tools/lighting.h"
#include "../tools/curve.h"
#include "../tools/euclid.h"

namespace paysages {
namespace basics {
    class NoiseGenerator;
}
}

typedef enum
{
    WATER_PRESET_LAKE,
    WATER_PRESET_SEA
} WaterPreset;

class WaterDefinition
{
public:
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
};

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

class WaterRenderer
{
public:
    WaterDefinition* definition;

    FuncWaterGetHeightInfo getHeightInfo;
    FuncWaterGetHeight getHeight;
    FuncWaterGetResult getResult;
};


RENDERINGSHARED_EXPORT extern StandardDefinition WaterDefinitionClass;
RENDERINGSHARED_EXPORT extern StandardRenderer WaterRendererClass;

RENDERINGSHARED_EXPORT void waterRenderSurface(Renderer* renderer);
RENDERINGSHARED_EXPORT void waterAutoPreset(WaterDefinition* definition, WaterPreset preset);

RENDERINGSHARED_EXPORT void waterAlterPreviewCoverageRenderer(Renderer* renderer);
RENDERINGSHARED_EXPORT Color waterGetPreviewCoverage(Renderer* renderer, double x, double y, double scaling, int highlight_enabled);

RENDERINGSHARED_EXPORT Renderer* waterCreatePreviewColorRenderer();
RENDERINGSHARED_EXPORT Color waterGetPreviewColor(Renderer* renderer, double x, double y, double scaling);

#endif
