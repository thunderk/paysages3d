#ifndef _PAYSAGES_SKY_H_
#define _PAYSAGES_SKY_H_

#include "shared/types.h"
#include "color.h"
#include "lighting.h"
#include "pack.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    float daytime;
    ColorGradation* sun_color;
    float sun_radius;
    float sun_halo_size;
    Curve* sun_halo_profile;
    ColorGradation* zenith_color;
    ColorGradation* haze_color;
    float haze_height;
    float haze_smoothing;
    ColorGradation* _sky_gradation;
} SkyDefinition;

void skyInit();
void skyQuit();
void skySave(PackStream* stream, SkyDefinition* definition);
void skyLoad(PackStream* stream, SkyDefinition* definition);

SkyDefinition skyCreateDefinition();
void skyDeleteDefinition(SkyDefinition* definition);
void skyCopyDefinition(SkyDefinition* source, SkyDefinition* destination);
void skyValidateDefinition(SkyDefinition* definition);

int skyGetLights(SkyDefinition* sky, LightDefinition* lights, int max_lights);
Color skyGetColor(SkyDefinition* definition, Renderer* renderer, Vector3 eye, Vector3 look);
void skyRender(SkyDefinition* definition, Renderer* renderer);
Vector3 skyGetSunDirection(SkyDefinition* definition);
Color skyGetSunColor(SkyDefinition* definition);
Color skyGetZenithColor(SkyDefinition* definition);

#ifdef __cplusplus
}
#endif

#endif
