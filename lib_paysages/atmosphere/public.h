#ifndef _PAYSAGES_ATMOSPHERE_PUBLIC_H_
#define _PAYSAGES_ATMOSPHERE_PUBLIC_H_

#include "../shared/types.h"
#include "../euclid.h"
#include "../color.h"
#include "../pack.h"
#include "../layers.h"

#ifdef __cplusplus
extern "C" {
#endif

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
Renderer atmosphereCreatePreviewRenderer();
Color atmosphereGetPreview(Renderer* renderer, double x, double y, double heading);

#ifdef __cplusplus
}
#endif

#endif
