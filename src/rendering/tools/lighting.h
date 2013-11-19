#ifndef _PAYSAGES_TOOLS_LIGHTING_H_
#define _PAYSAGES_TOOLS_LIGHTING_H_

#include "../rendering_global.h"
#include "euclid.h"
#include "Color.h"

typedef struct
{
    Vector3 direction;  /* Global direction of the light */
    Color color;        /* Main color of the light */
    double reflection;  /* Reflected factor of the light (for specular lighting) */
    int altered;        /* Should the light be filtered or masked (by atmosphere, water, ground, clouds...) */
} LightDefinition;

typedef int (*FuncLightingAlterLight)(void* data, LightDefinition* light, Vector3 at);

class LightingManager;
typedef struct LightStatus LightStatus;

RENDERINGSHARED_EXPORT LightingManager* lightingManagerCreate();
RENDERINGSHARED_EXPORT void lightingManagerDelete(LightingManager* filter);
RENDERINGSHARED_EXPORT void lightingManagerRegisterFilter(LightingManager* filter, FuncLightingAlterLight callback, void* data);
RENDERINGSHARED_EXPORT void lightingManagerDisableSpecularity(LightingManager* manager);

RENDERINGSHARED_EXPORT LightStatus* lightingCreateStatus(LightingManager* manager, Vector3 location, Vector3 eye);
RENDERINGSHARED_EXPORT void lightingDeleteStatus(LightStatus* status);
RENDERINGSHARED_EXPORT void lightingPushLight(LightStatus* status, LightDefinition* light);
RENDERINGSHARED_EXPORT Color lightingApplyStatus(LightStatus* status, Vector3 normal, SurfaceMaterial* material);
RENDERINGSHARED_EXPORT Vector3 lightingGetStatusLocation(LightStatus* status);

RENDERINGSHARED_EXPORT Color lightingApplyOneLight(LightDefinition* light, Vector3 eye, Vector3 location, Vector3 normal, SurfaceMaterial* material);

#endif
