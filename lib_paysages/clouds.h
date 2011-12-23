#ifndef _PAYSAGES_CLOUDS_H_
#define _PAYSAGES_CLOUDS_H_

#include "shared/types.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    double ycenter;
    double ymin;
    double ymax;
    NoiseGenerator* noise;
    Color color;
    double scaling;
    double coverage;
} CloudsDefinition;

typedef struct
{
    double precision;
} CloudsQuality;

typedef struct
{
    int unused;
} CloudsEnvironment;

void cloudsInit();
void cloudsSave(FILE* f);
void cloudsLoad(FILE* f);

int cloudsGetLayerCount();
int cloudsAddLayer();
void cloudsDeleteLayer(int layer);

CloudsDefinition cloudsCreateDefinition();
void cloudsDeleteDefinition(CloudsDefinition definition);
void cloudsCopyDefinition(CloudsDefinition source, CloudsDefinition* destination);
void cloudsSetDefinition(int layer, CloudsDefinition definition);
CloudsDefinition cloudsGetDefinition(int layer);

void cloudsSetQuality(CloudsQuality quality);
CloudsQuality cloudsGetQuality();

Color cloudsGetColorCustom(Vector3 start, Vector3 end, CloudsDefinition* definition, CloudsQuality* quality, CloudsEnvironment* environment);
Color cloudsGetColor(Vector3 start, Vector3 end);

#ifdef __cplusplus
}
#endif

#endif
