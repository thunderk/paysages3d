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

typedef struct
{
    double ycenter;
    double ymin;
    double ymax;
    NoiseGenerator* noise;
    SurfaceMaterial material;
    double transparencydepth;
    double lighttraversal;
    double minimumlight;
    double scaling;
    double coverage;
} CloudsLayerDefinition;

typedef struct
{
    int nblayers;
    CloudsLayerDefinition layers[CLOUDS_MAX_LAYERS];
} CloudsDefinition;

void cloudsInit();
void cloudsSave(FILE* f, CloudsDefinition* definition);
void cloudsLoad(FILE* f, CloudsDefinition* definition);

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

Color cloudsGetLayerColor(CloudsLayerDefinition* definition, Renderer* renderer, Vector3 start, Vector3 end);
Color cloudsGetColor(CloudsDefinition* definition, Renderer* renderer, Vector3 start, Vector3 end);
Color cloudsLayerFilterLight(CloudsLayerDefinition* definition, Renderer* renderer, Color light, Vector3 location, Vector3 light_location, Vector3 direction_to_light);
Color cloudsFilterLight(CloudsDefinition* definition, Renderer* renderer, Color light, Vector3 location, Vector3 light_location, Vector3 direction_to_light);

#ifdef __cplusplus
}
#endif

#endif
