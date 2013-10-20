#ifndef _PAYSAGES_TOOLS_LIGHTING_H_
#define _PAYSAGES_TOOLS_LIGHTING_H_

#include "../rendering_global.h"
#include "euclid.h"
#include "color.h"
#include "pack.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    ColorHSL base;

    double hardness;
    double reflection;
    double shininess;

    double receive_shadows;

    Color _rgb;
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

RENDERINGSHARED_EXPORT void materialSave(PackStream* stream, SurfaceMaterial* material);
RENDERINGSHARED_EXPORT void materialLoad(PackStream* stream, SurfaceMaterial* material);
RENDERINGSHARED_EXPORT void materialValidate(SurfaceMaterial* material);

#ifdef __cplusplus
}
#endif

#endif
