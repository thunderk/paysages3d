#ifndef _PAYSAGES_CLOUDS_PUBLIC_H_
#define _PAYSAGES_CLOUDS_PUBLIC_H_

#include "../rendering_global.h"

#include "../shared/types.h"
#include "../tools/lighting.h"
#include "../tools/euclid.h"
#include "SurfaceMaterial.h"

typedef Color (*FuncCloudsGetColor)(Renderer* renderer, Color base, Vector3 start, Vector3 end);
typedef double (*FuncCloudsGetLayerDensity)(Renderer* renderer, CloudLayerDefinition* layer, Vector3 location);
typedef double (*FuncCloudsGetEdgeDensity)(Renderer* renderer, CloudLayerDefinition* layer, Vector3 location, double layer_density);

class CloudsRenderer
{
public:
    CloudsDefinition* definition;

    FuncCloudsGetColor getColor;
    FuncLightingAlterLight alterLight;
    FuncCloudsGetLayerDensity getLayerDensity;
    FuncCloudsGetEdgeDensity getEdgeDensity;
};


RENDERINGSHARED_EXPORT extern StandardRenderer CloudsRendererClass;

#endif
