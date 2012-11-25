#ifndef _PAYSAGES_ATMOSPHERE_INTERFACE_H_
#define _PAYSAGES_ATMOSPHERE_INTERFACE_H_

#include "../shared/types.h"
#include "../euclid.h"
#include "../color.h"
#include "../pack.h"
#include "../layers.h"

#ifdef __cplusplus
extern "C" {
#endif

    /*** TO EXTRACT ***/
typedef void* (*FuncObjectCreate)();
typedef void (*FuncObjectDelete)(void* object);
typedef void (*FuncObjectCopy)(void* source, void* destination);
typedef void (*FuncObjectValidate)(void* object);
typedef void (*FuncObjectSave)(PackStream* stream, void* object);
typedef void (*FuncObjectLoad)(PackStream* stream, void* object);
typedef void (*FuncObjectBind)(void* base, void* sub);
    
typedef struct {
    FuncObjectCreate create;
    FuncObjectDelete destroy;
    FuncObjectCopy copy;
    FuncObjectValidate validate;
    FuncObjectSave save;
    FuncObjectLoad load;
} StandardDefinition;

typedef struct {
    FuncObjectCreate create;
    FuncObjectDelete destroy;
    FuncObjectBind bind;
} StandardRenderer;
    /*** TO EXTRACT ***/

typedef enum
{
    ATMOSPHERE_MODEL_PREETHAM = 0,
    ATMOSPHERE_MODEL_BRUNETON = 1
} AtmosphereModel;

typedef struct
{
    AtmosphereModel model;
    double daytime;
    double humidity;
    Color sun_color;
    double sun_radius;
    double sun_halo_size;
    Curve* sun_halo_profile;
    double dome_lighting;
} AtmosphereDefinition;

typedef int (*FuncAtmosphereGetSkydomeLights)(Renderer* renderer, LightDefinition* array, int max_lights);
typedef Color (*FuncAtmosphereApplyAerialPerspective)(Renderer* renderer, Vector3 location, Color base);
typedef Color (*FuncAtmosphereGetSkyColor)(Renderer* renderer, Vector3 direction);
typedef Vector3 (*FuncAtmosphereGetSunDirection)(Renderer* renderer);

typedef struct
{
    AtmosphereDefinition* definition;
    
    FuncAtmosphereGetSkydomeLights getSkydomeLights;
    FuncAtmosphereApplyAerialPerspective applyAerialPerspective;
    FuncAtmosphereGetSkyColor getSkyColor;
    FuncAtmosphereGetSunDirection getSunDirection;
    
    void* _internal_data;
} AtmosphereRenderer;

extern StandardDefinition AtmosphereDefinitionClass;
extern StandardRenderer AtmosphereRendererClass;

void atmosphereRenderSkydome(Renderer* renderer);

#ifdef __cplusplus
}
#endif

#endif
