#ifndef _PAYSAGES_SKY_H_
#define _PAYSAGES_SKY_H_

#include "shared/types.h"
#include "color.h"
#include "lighting.h"
#include "pack.h"

#ifdef __cplusplus
extern "C" {
#endif
    
typedef enum
{
    SKY_MODEL_CUSTOM = 0,
    SKY_MODEL_RAYLEIGH_MIE = 1,
    SKY_MODEL_PREETHAM = 2
} SkyModel;

typedef struct
{
    SkyModel model;
    double daytime;
    Color sun_color;
    double sun_radius;
    double sun_halo_size;
    Curve* sun_halo_profile;
    struct {
        int auto_from_daytime;
        Color zenith_color;
        Color haze_color;
        double haze_height;
        double haze_smoothing;
        ColorGradation* _sky_gradation;
    } model_custom;
    struct {
        double turbidity;
    } model_preetham;
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
Color skyGetHorizonColor(SkyDefinition* definition);

#ifdef __cplusplus
}
#endif

#endif
