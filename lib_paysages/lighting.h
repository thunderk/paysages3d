#ifndef _PAYSAGES_LIGHTING_H_
#define _PAYSAGES_LIGHTING_H_

#include "shared/types.h"
#include "pack.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LIGHTING_MAX_LIGHTS 30

struct LightDefinition
{
    Vector3 direction;  /* Global direction of the light */
    Color color;        /* Main color of the light */
    double reflection;  /* Reflected factor of the light (for specular lighting) */
    int filtered;       /* Should the light be filtered (by atmosphere, water...) */
    int masked;         /* Should the light be masked (cast shadows..) */
};

typedef struct
{
    int nblights;
    LightDefinition lights[LIGHTING_MAX_LIGHTS];
} LightingDefinition;

struct LightStatus
{
    int nblights;
    LightDefinition lights[LIGHTING_MAX_LIGHTS * 2];
};

typedef LightDefinition (*FuncLightingAlterLight)(Renderer* renderer, LightDefinition* light, Vector3 at);

void lightingInit();
void lightingQuit();
void lightingSave(PackStream* stream, LightingDefinition* definition);
void lightingLoad(PackStream* stream, LightingDefinition* definition);

LightingDefinition lightingCreateDefinition();
void lightingDeleteDefinition(LightingDefinition* definition);
void lightingCopyDefinition(LightingDefinition* source, LightingDefinition* destination);
void lightingValidateDefinition(LightingDefinition* definition);

int lightingGetLightCount(LightingDefinition* definition);
LightDefinition lightingGetLight(LightingDefinition* definition, int light);
int lightingAddLight(LightingDefinition* definition, LightDefinition light);
void lightingDeleteLight(LightingDefinition* definition, int light);

void lightingGetStatus(LightingDefinition* definition, Renderer* renderer, Vector3 location, LightStatus* result);
Color lightingApplyStatusToSurface(Renderer* renderer, LightStatus* status, Vector3 location, Vector3 normal, SurfaceMaterial material);
Color lightingApplyToSurface(LightingDefinition* definition, Renderer* renderer, Vector3 location, Vector3 normal, SurfaceMaterial material);

#ifdef __cplusplus
}
#endif

#endif
