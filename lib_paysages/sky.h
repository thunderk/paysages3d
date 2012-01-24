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

void skyInit();
void skySave(FILE* f, SkyDefinition* definition);
void skyLoad(FILE* f, SkyDefinition* definition);

SkyDefinition skyCreateDefinition();
void skyDeleteDefinition(SkyDefinition* definition);
void skyCopyDefinition(SkyDefinition* source, SkyDefinition* destination);
void skyValidateDefinition(SkyDefinition* definition);

int skyGetLights(SkyDefinition* sky, LightDefinition* lights, int max_lights);
Color skyGetColor(SkyDefinition* definition, Renderer* renderer, Vector3 eye, Vector3 look);
void skyRender(SkyDefinition* definition, Renderer* renderer, RenderProgressCallback callback);

#ifdef __cplusplus
}
#endif

#endif
