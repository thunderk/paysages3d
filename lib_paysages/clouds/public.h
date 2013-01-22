#ifndef _PAYSAGES_CLOUDS_PUBLIC_H_
#define _PAYSAGES_CLOUDS_PUBLIC_H_

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
    CLOUDS_TYPE_CIRRUS,
    CLOUDS_TYPE_CUMULUS,
    CLOUDS_TYPE_STRATOCUMULUS,
    CLOUDS_TYPE_STRATUS
} CloudsType;

typedef enum
{
    CLOUDS_PRESET_CIRRUS,
    CLOUDS_PRESET_CUMULUS,
    CLOUDS_PRESET_STRATOCUMULUS,
    CLOUDS_PRESET_STRATUS
} CloudsPreset;

typedef struct
{
    CloudsType type;
    double lower_altitude;
    double thickness;
    double base_coverage;
    double shape_scaling;
    double edge_scaling;
    double edge_length;
    SurfaceMaterial material;
    double hardness;
    double transparencydepth;
    double lighttraversal;
    double minimumlight;

    Curve* _coverage_by_altitude;
    NoiseGenerator* _shape_noise;
    NoiseGenerator* _edge_noise;
} CloudsLayerDefinition;

typedef struct
{
    Layers* layers;
} CloudsDefinition;


typedef Color (*FuncCloudsGetColor)(Renderer* renderer, Color base, Vector3 start, Vector3 end);

typedef struct
{
    CloudsDefinition* definition;

    FuncCloudsGetColor getColor;
    FuncLightingAlterLight alterLight;
} CloudsRenderer;


extern StandardDefinition CloudsDefinitionClass;
extern StandardRenderer CloudsRendererClass;


LayerType cloudsGetLayerType();
void cloudsAutoPreset(CloudsLayerDefinition* definition, CloudsPreset preset);

Renderer* cloudsCreatePreviewCoverageRenderer();
Color cloudsGetPreviewCoverage(Renderer* renderer, double x, double y, double scaling);

Renderer* cloudsCreatePreviewColorRenderer();
Color cloudsGetPreviewColor(Renderer* renderer, double x, double y);

#ifdef __cplusplus
}
#endif

#endif
