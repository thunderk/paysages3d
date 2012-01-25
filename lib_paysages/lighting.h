#ifndef _PAYSAGES_LIGHTING_H_
#define _PAYSAGES_LIGHTING_H_

#include "shared/types.h"
#include "renderer.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LIGHTING_MAX_LIGHTS 10

typedef struct
{
    Vector3 direction;  /* Global direction of the light */
    Color color;        /* Main color of the light */
    int filtered;       /* Should the light be filtered (by atmosphere, water...) */
    int masked;         /* Should the light be masked (cast shadows..) */
    double amplitude;   /* Angle amplitude of the light source (for multi-sampling, pi / 2.0 for skydome) */
} LightDefinition;

typedef struct
{
    int autosetfromsky;
    int nblights;
    LightDefinition lights[LIGHTING_MAX_LIGHTS];
    int _nbautolights;
    LightDefinition _autolights[LIGHTING_MAX_LIGHTS];
} LightingDefinition;

void lightingInit();
void lightingSave(FILE* f, LightingDefinition* definition);
void lightingLoad(FILE* f, LightingDefinition* definition);

LightingDefinition lightingCreateDefinition();
void lightingDeleteDefinition(LightingDefinition* definition);
void lightingCopyDefinition(LightingDefinition* source, LightingDefinition* destination);
void lightingValidateDefinition(LightingDefinition* definition);

int lightingGetLightCount(LightingDefinition* definition);
LightDefinition lightingGetLight(LightingDefinition* definition, int light);
int lightingAddLight(LightingDefinition* definition, LightDefinition light);
void lightingDeleteLight(LightingDefinition* definition, int light);

Color lightingApplyToSurface(LightingDefinition* definition, Renderer* renderer, Vector3 location, Vector3 normal, SurfaceMaterial material);

#ifdef __cplusplus
}
#endif

#endif