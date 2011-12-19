#ifndef _PAYSAGES_WATER_H_
#define _PAYSAGES_WATER_H_

#include "shared/types.h"
#include <stdio.h>

typedef struct
{
    double height;
    double transparency;
    double reflection;
    Color main_color;
    Color depth_color;
    double transparency_depth;
    NoiseGenerator* height_noise;
} WaterDefinition;

typedef struct
{
    double detail_boost;
    double force_detail;
} WaterQuality;

typedef struct
{
    RayCastingFunction reflection_function;
    RayCastingFunction refraction_function;
    int toggle_fog;
    int toggle_shadows;
} WaterEnvironment;

typedef struct
{
    Vector3 location;
    Color base;
    Color reflected;
    Color refracted;
    Color final;
} WaterResult;

void waterInit();
void waterSave(FILE* f);
void waterLoad(FILE* f);

WaterDefinition waterCreateDefinition();
void waterDeleteDefinition(WaterDefinition definition);
void waterCopyDefinition(WaterDefinition source, WaterDefinition* destination);
void waterSetDefinition(WaterDefinition definition);
WaterDefinition waterGetDefinition();
void waterSetQuality(WaterQuality quality);
WaterQuality waterGetQuality();

double waterGetLightFactor(Vector3 location);
WaterResult waterGetColorCustom(Vector3 location, Vector3 look, WaterDefinition* definition, WaterQuality* quality, WaterEnvironment* environment);
Color waterGetColor(Vector3 location, Vector3 look);
void waterRender(RenderProgressCallback callback);

#endif
