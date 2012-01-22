#ifndef _PAYSAGES_SKY_H_
#define _PAYSAGES_SKY_H_

#include "shared/types.h"
#include "lighting.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    double daytime;
    ColorGradation sun_color;
    double sun_radius;
    ColorGradation zenith_color;
    ColorGradation haze_color;
    double haze_height;
    double haze_smoothing;
    ColorGradation _sky_gradation;
} SkyDefinition;

typedef struct
{
    int unused;
} SkyQuality;

typedef struct
{
    int unused;
} SkyEnvironment;

void skyInit();
void skySave(FILE* f);
void skyLoad(FILE* f);

SkyDefinition skyCreateDefinition();
void skyDeleteDefinition(SkyDefinition definition);
void skyCopyDefinition(SkyDefinition source, SkyDefinition* destination);
void skyValidateDefinition(SkyDefinition* definition);
void skySetDefinition(SkyDefinition definition);
SkyDefinition skyGetDefinition();

void skySetQuality(SkyQuality quality);
SkyQuality skyGetQuality();

int skyGetLights(LightDefinition* lights, int max_lights);

Color skyGetColorCustom(Vector3 eye, Vector3 look, SkyDefinition* definition, SkyQuality* quality, SkyEnvironment* environment);
Color skyGetColor(Vector3 eye, Vector3 look);

Color skyProjectRay(Vector3 start, Vector3 direction);
void skyRender(RenderProgressCallback callback);

#ifdef __cplusplus
}
#endif

#endif
