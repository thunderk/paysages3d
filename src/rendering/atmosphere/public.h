#ifndef _PAYSAGES_ATMOSPHERE_PUBLIC_H_
#define _PAYSAGES_ATMOSPHERE_PUBLIC_H_

#include "rendering_global.h"

#include "../rendering_global.h"
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

typedef AtmosphereResult (*FuncAtmosphereApplyAerialPerspective)(Renderer* renderer, Vector3 location, Color base);
typedef AtmosphereResult (*FuncAtmosphereGetSkyColor)(Renderer* renderer, Vector3 direction);
typedef Vector3 (*FuncAtmosphereGetSunDirection)(Renderer* renderer);

class AtmosphereRenderer
{
public:
    AtmosphereDefinition* definition;

    FuncAtmosphereApplyAerialPerspective applyAerialPerspective;
    FuncAtmosphereGetSkyColor getSkyColor;
    FuncAtmosphereGetSunDirection getSunDirection;

    /*void* _internal_data;*/
};

RENDERINGSHARED_EXPORT extern StandardRenderer AtmosphereRendererClass;

RENDERINGSHARED_EXPORT void atmosphereInitResult(AtmosphereResult* result);
RENDERINGSHARED_EXPORT void atmosphereUpdateResult(AtmosphereResult* result);

#endif
