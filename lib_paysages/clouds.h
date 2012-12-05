#ifndef _PAYSAGES_CLOUDS_H_
#define _PAYSAGES_CLOUDS_H_

#include "shared/types.h"
#include "layers.h"
#include "noise.h"
#include "renderer.h"

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

typedef struct CloudsLayerDefinition CloudsLayerDefinition;
    
typedef double (*CloudCoverageFunc)(CloudsLayerDefinition* definition, Vector3 position);

struct CloudsLayerDefinition
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
    
    CloudCoverageFunc _custom_coverage;
    Curve* _coverage_by_altitude;
    NoiseGenerator* _shape_noise;
    NoiseGenerator* _edge_noise;
};

typedef struct
{
    Layers* layers;
} CloudsDefinition;

CloudsDefinition cloudsCreateDefinition();
void cloudsDeleteDefinition(CloudsDefinition* definition);
void cloudsCopyDefinition(CloudsDefinition* source, CloudsDefinition* destination);
void cloudsValidateDefinition(CloudsDefinition* definition);
void cloudsSave(PackStream* stream, CloudsDefinition* definition);
void cloudsLoad(PackStream* stream, CloudsDefinition* definition);

CloudsLayerDefinition* cloudsLayerCreateDefinition();
void cloudsLayerDeleteDefinition(CloudsLayerDefinition* definition);
void cloudsLayerAutoPreset(CloudsLayerDefinition* definition, CloudsPreset preset);
void cloudsLayerCopyDefinition(CloudsLayerDefinition* source, CloudsLayerDefinition* destination);
void cloudsLayerValidateDefinition(CloudsLayerDefinition* definition);
void cloudsLayerSetName(CloudsLayerDefinition* definition, const char* name);
LayerType cloudsGetLayerType();

Color cloudsApplyLayer(CloudsLayerDefinition* definition, Color base, Renderer* renderer, Vector3 start, Vector3 end);
Color cloudsApply(CloudsDefinition* definition, Color base, Renderer* renderer, Vector3 start, Vector3 end);
Color cloudsLayerFilterLight(CloudsLayerDefinition* definition, Renderer* renderer, Color light, Vector3 location, Vector3 light_location, Vector3 direction_to_light);
Color cloudsFilterLight(CloudsDefinition* definition, Renderer* renderer, Color light, Vector3 location, Vector3 light_location, Vector3 direction_to_light);

#ifdef __cplusplus
}
#endif

#endif
