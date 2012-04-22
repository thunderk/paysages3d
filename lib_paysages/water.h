#ifndef _PAYSAGES_WATER_H_
#define _PAYSAGES_WATER_H_

#include "shared/types.h"
#include "renderer.h"
#include "lighting.h"
#include "noise.h"
#include "pack.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    double height;
    double transparency;
    double reflection;
    SurfaceMaterial material;
    Color depth_color;
    double transparency_depth;
    double lighting_depth;
    NoiseGenerator* waves_noise;
    double waves_noise_height;
    double waves_noise_scale;
} WaterDefinition;

typedef struct
{
    Vector3 location;
    Color base;
    Color reflected;
    Color refracted;
    Color final;
} WaterResult;

void waterInit();
void waterQuit();
void waterSave(PackStream* stream, WaterDefinition* definition);
void waterLoad(PackStream* stream, WaterDefinition* definition);

WaterDefinition waterCreateDefinition();
void waterDeleteDefinition(WaterDefinition* definition);
void waterCopyDefinition(WaterDefinition* source, WaterDefinition* destination);
void waterValidateDefinition(WaterDefinition* definition);

HeightInfo waterGetHeightInfo(WaterDefinition* definition);
Color waterLightFilter(WaterDefinition* definition, Renderer* renderer, Color light, Vector3 location, Vector3 light_location, Vector3 direction_to_light);
WaterResult waterGetColorDetail(WaterDefinition* definition, Renderer* renderer, Vector3 location, Vector3 look);
Color waterGetColor(WaterDefinition* definition, Renderer* renderer, Vector3 location, Vector3 look);
void waterRender(WaterDefinition* definition, Renderer* renderer);

#ifdef __cplusplus
}
#endif

#endif
