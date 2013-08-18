#ifndef _PAYSAGES_TOOLS_LIGHTING_H_
#define _PAYSAGES_TOOLS_LIGHTING_H_

#include "euclid.h"
#include "color.h"
#include "pack.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    double hue;
    double diffuse;
    double hardness;

    double reflection;
    double shininess;

    double receive_shadows;

    Color base;
} SurfaceMaterial;

typedef struct
{
    Vector3 direction;  /* Global direction of the light */
    Color color;        /* Main color of the light */
    double reflection;  /* Reflected factor of the light (for specular lighting) */
    int altered;        /* Should the light be filtered or masked (by atmosphere, water, ground, clouds...) */
} LightDefinition;

typedef int (*FuncLightingAlterLight)(void* data, LightDefinition* light, Vector3 at);

typedef struct LightingManager LightingManager;
typedef struct LightStatus LightStatus;

LightingManager* lightingManagerCreate();
void lightingManagerDelete(LightingManager* filter);
void lightingManagerRegisterFilter(LightingManager* filter, FuncLightingAlterLight callback, void* data);
void lightingManagerDisableSpecularity(LightingManager* manager);

LightStatus* lightingCreateStatus(LightingManager* manager, Vector3 location, Vector3 eye);
void lightingDeleteStatus(LightStatus* status);
void lightingPushLight(LightStatus* status, LightDefinition* light);
Color lightingApplyStatus(LightStatus* status, Vector3 normal, SurfaceMaterial* material);
Vector3 lightingGetStatusLocation(LightStatus* status);

Color lightingApplyOneLight(LightDefinition* light, Vector3 eye, Vector3 location, Vector3 normal, SurfaceMaterial* material);

void materialSave(PackStream* stream, SurfaceMaterial* material);
void materialLoad(PackStream* stream, SurfaceMaterial* material);

#ifdef __cplusplus
}
#endif

#endif
