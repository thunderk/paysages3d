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
    ATMOSPHERE_PRESET_CLEAR_DAY = 0,
    ATMOSPHERE_PRESET_CLEAR_SUNSET = 1,
    ATMOSPHERE_PRESET_HAZY_MORNING = 2,
    ATMOSPHERE_PRESET_FOGGY = 3,
    ATMOSPHERE_PRESET_STORMY = 4
} AtmospherePreset;

typedef enum
{
    ATMOSPHERE_MODEL_PREETHAM = 0,
    ATMOSPHERE_MODEL_BRUNETON = 1
} AtmosphereModel;

typedef struct
{
    AtmosphereModel model;
    int hour;
    int minute;
    double humidity;
    Color sun_color;
    double sun_radius;
    double dome_lighting;

    double _daytime;
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

void atmosphereAutoPreset(AtmosphereDefinition* definition, AtmospherePreset preset);
void atmosphereRenderSkydome(Renderer* renderer);
Renderer atmosphereCreatePreviewRenderer();
Color atmosphereGetPreview(Renderer* renderer, double x, double y, double heading);

#ifdef __cplusplus
}
#endif

#endif
