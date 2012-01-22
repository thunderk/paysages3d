#ifndef _PAYSAGES_LIGHTING_H_
#define _PAYSAGES_LIGHTING_H_

#include "shared/types.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_LIGHTS 10

typedef struct
{
    Vector3 direction;
    Color color;
    double maxshadow;
} LightDefinition;

typedef struct
{
    int autosetfromsky;
    int nblights;
    LightDefinition lights[MAX_LIGHTS];
    int _nbautolights;
    LightDefinition _autolights[MAX_LIGHTS];
} LightingDefinition;

typedef struct
{
    int unused;
} LightingQuality;

typedef Color (*LightFilter)(Color light, Vector3 location, Vector3 light_location, Vector3 direction_to_light, void* custom_data);

typedef struct
{
    LightFilter filter;
    void* custom_data;
} LightingEnvironment;

typedef struct
{
    Color base;
    double reflection;
    double shininess;
} ReceiverMaterial;

void lightingInit();
void lightingSave(FILE* f);
void lightingLoad(FILE* f);

LightingDefinition lightingCreateDefinition();
void lightingDeleteDefinition(LightingDefinition definition);
void lightingCopyDefinition(LightingDefinition source, LightingDefinition* destination);
void lightingSetDefinition(LightingDefinition definition);
LightingDefinition lightingGetDefinition();
void lightingValidateDefinition(LightingDefinition* definition);

int lightingGetLightCount(LightingDefinition* definition);
LightDefinition lightingGetLight(LightingDefinition* definition, int light);
int lightingAddLight(LightingDefinition* definition, LightDefinition light);
void lightingDeleteLight(LightingDefinition* definition, int light);

void lightingSetQuality(LightingQuality quality);
LightingQuality lightingGetQuality();

Color lightingApplyCustom(Vector3 location, Vector3 normal, ReceiverMaterial material, LightingDefinition* definition, LightingQuality* quality, LightingEnvironment* environment);
Color lightingApply(Vector3 location, Vector3 normal, ReceiverMaterial material);

#ifdef __cplusplus
}
#endif

#endif
