#ifndef _PAYSAGES_CLOUDS_PUBLIC_H_
#define _PAYSAGES_CLOUDS_PUBLIC_H_

#include "../rendering_global.h"
#include "../shared/types.h"
#include "../tools/lighting.h"
#include "../tools/curve.h"
#include "../tools/euclid.h"
#include "Layers.h"

namespace paysages {
namespace basics {
    class NoiseGenerator;
}
}

typedef enum
{
    CLOUDS_TYPE_CIRRUS,
    CLOUDS_TYPE_CUMULUS,
    CLOUDS_TYPE_STRATOCUMULUS,
    CLOUDS_TYPE_STRATUS
} CloudsType;

typedef enum
{
    CLOUDS_PRESET_PARTLY_CLOUDY,
} CloudsPreset;

typedef enum
{
    CLOUDS_LAYER_PRESET_CIRRUS,
    CLOUDS_LAYER_PRESET_CUMULUS,
    CLOUDS_LAYER_PRESET_STRATOCUMULUS,
    CLOUDS_LAYER_PRESET_STRATUS
} CloudsLayerPreset;

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
    NoiseGenerator* _coverage_noise;
    NoiseGenerator* _shape_noise;
    NoiseGenerator* _edge_noise;
} CloudsLayerDefinition;

typedef struct
{
    Layers* layers;
} CloudsDefinition;

typedef Color (*FuncCloudsGetColor)(Renderer* renderer, Color base, Vector3 start, Vector3 end);
typedef double (*FuncCloudsGetLayerDensity)(Renderer* renderer, CloudsLayerDefinition* layer, Vector3 location);
typedef double (*FuncCloudsGetEdgeDensity)(Renderer* renderer, CloudsLayerDefinition* layer, Vector3 location, double layer_density);

typedef struct
{
    CloudsDefinition* definition;

    FuncCloudsGetColor getColor;
    FuncLightingAlterLight alterLight;
    FuncCloudsGetLayerDensity getLayerDensity;
    FuncCloudsGetEdgeDensity getEdgeDensity;
} CloudsRenderer;


RENDERINGSHARED_EXPORT extern StandardDefinition CloudsDefinitionClass;
RENDERINGSHARED_EXPORT extern StandardRenderer CloudsRendererClass;

RENDERINGSHARED_EXPORT LayerType cloudsGetLayerType();
RENDERINGSHARED_EXPORT void cloudsAutoPreset(CloudsDefinition* definition, CloudsPreset preset);
RENDERINGSHARED_EXPORT void cloudsLayerAutoPreset(CloudsLayerDefinition* definition, CloudsLayerPreset preset);

#endif
