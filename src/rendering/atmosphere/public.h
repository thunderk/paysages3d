#ifndef _PAYSAGES_ATMOSPHERE_PUBLIC_H_
#define _PAYSAGES_ATMOSPHERE_PUBLIC_H_

#include "../rendering_global.h"
#include "../tools/lighting.h"
#include "../tools/euclid.h"
#include "../tools/color.h"
#include "../shared/types.h"

namespace paysages {
namespace system {class PackStream;}
}

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
    ATMOSPHERE_MODEL_BRUNETON = 0
} AtmosphereModel;

class AtmosphereDefinition
{
public:
    AtmosphereModel model;
    int hour;
    int minute;
    double humidity;
    Color sun_color;
    double sun_radius;
    double dome_lighting;

    double _daytime;
};

typedef struct
{
    Color base;
    double distance;
    Color inscattering;
    Color attenuation;
    Color mask;
    Color final;
} AtmosphereResult;

typedef void (*FuncAtmosphereGetLightingStatus)(Renderer* renderer, LightStatus* status, Vector3 normal, int opaque);
typedef AtmosphereResult (*FuncAtmosphereApplyAerialPerspective)(Renderer* renderer, Vector3 location, Color base);
typedef AtmosphereResult (*FuncAtmosphereGetSkyColor)(Renderer* renderer, Vector3 direction);
typedef Vector3 (*FuncAtmosphereGetSunDirection)(Renderer* renderer);

class AtmosphereRenderer
{
public:
    AtmosphereDefinition* definition;

    FuncAtmosphereGetLightingStatus getLightingStatus;
    FuncAtmosphereApplyAerialPerspective applyAerialPerspective;
    FuncAtmosphereGetSkyColor getSkyColor;
    FuncAtmosphereGetSunDirection getSunDirection;

    /*void* _internal_data;*/
};

RENDERINGSHARED_EXPORT extern StandardDefinition AtmosphereDefinitionClass;
RENDERINGSHARED_EXPORT extern StandardRenderer AtmosphereRendererClass;

RENDERINGSHARED_EXPORT void atmosphereAutoPreset(AtmosphereDefinition* definition, AtmospherePreset preset);

RENDERINGSHARED_EXPORT void atmosphereRenderSkydome(Renderer* renderer);

RENDERINGSHARED_EXPORT void atmosphereInitResult(AtmosphereResult* result);
RENDERINGSHARED_EXPORT void atmosphereUpdateResult(AtmosphereResult* result);

#endif
