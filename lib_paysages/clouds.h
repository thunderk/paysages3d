#ifndef _PAYSAGES_CLOUDS_H_
#define _PAYSAGES_CLOUDS_H_

#include "shared/types.h"
#include "noise.h"
#include "renderer.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CLOUDS_MAX_LAYERS 6

typedef struct CloudsLayerDefinition CloudsLayerDefinition;
    
typedef double (*CloudCoverageFunc)(CloudsLayerDefinition* definition, Vector3 position);

struct CloudsLayerDefinition
{
    double lower_altitude;
    double thickness;
    double base_coverage;
    Curve* coverage_by_altitude;
    NoiseGenerator* shape_noise;
    double shape_scaling;
    NoiseGenerator* edge_noise;
    double edge_scaling;
    double edge_length;
    SurfaceMaterial material;
    double hardness;
    double transparencydepth;
    double lighttraversal;
    double minimumlight;
    CloudCoverageFunc _custom_coverage;
};

typedef struct
{
    int nblayers;
    CloudsLayerDefinition layers[CLOUDS_MAX_LAYERS];
} CloudsDefinition;

void cloudsInit();
void cloudsQuit();
void cloudsSave(PackStream* stream, CloudsDefinition* definition);
void cloudsLoad(PackStream* stream, CloudsDefinition* definition);

CloudsDefinition cloudsCreateDefinition();
void cloudsDeleteDefinition(CloudsDefinition* definition);
void cloudsCopyDefinition(CloudsDefinition* source, CloudsDefinition* destination);
void cloudsValidateDefinition(CloudsDefinition* definition);

CloudsLayerDefinition cloudsLayerCreateDefinition();
void cloudsLayerDeleteDefinition(CloudsLayerDefinition* definition);
void cloudsLayerCopyDefinition(CloudsLayerDefinition* source, CloudsLayerDefinition* destination);
void cloudsLayerValidateDefinition(CloudsLayerDefinition* definition);

int cloudsGetLayerCount(CloudsDefinition* definition);
CloudsLayerDefinition* cloudsGetLayer(CloudsDefinition* definition, int layer);
int cloudsAddLayer(CloudsDefinition* definition);
void cloudsDeleteLayer(CloudsDefinition* definition, int layer);
void cloudsMoveLayer(CloudsDefinition* definition, int layer, int new_position);

Color cloudsApplyLayer(CloudsLayerDefinition* definition, Color base, Renderer* renderer, Vector3 start, Vector3 end);
Color cloudsApply(CloudsDefinition* definition, Color base, Renderer* renderer, Vector3 start, Vector3 end);
Color cloudsLayerFilterLight(CloudsLayerDefinition* definition, Renderer* renderer, Color light, Vector3 location, Vector3 light_location, Vector3 direction_to_light);
Color cloudsFilterLight(CloudsDefinition* definition, Renderer* renderer, Color light, Vector3 location, Vector3 light_location, Vector3 direction_to_light);

#ifdef __cplusplus
}
#endif

#endif
