#ifndef _PAYSAGES_ATMOSPHERE_PUBLIC_H_
#define _PAYSAGES_ATMOSPHERE_PUBLIC_H_

#include "rendering_global.h"

#include "../rendering_global.h"
#include "../tools/lighting.h"
#include "../tools/euclid.h"
#include "../shared/types.h"
#include "Color.h"

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

RENDERINGSHARED_EXPORT extern StandardRenderer AtmosphereRendererClass;

RENDERINGSHARED_EXPORT void atmosphereRenderSkydome(Renderer* renderer);

RENDERINGSHARED_EXPORT void atmosphereInitResult(AtmosphereResult* result);
RENDERINGSHARED_EXPORT void atmosphereUpdateResult(AtmosphereResult* result);

#endif
