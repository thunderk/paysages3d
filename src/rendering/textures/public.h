#ifndef _PAYSAGES_TEXTURES_PUBLIC_H_
#define _PAYSAGES_TEXTURES_PUBLIC_H_

#include "../rendering_global.h"
#include "Layers.h"
#include "tools/lighting.h"
#include "terrain/public.h"
#include "SurfaceMaterial.h"

#define TEXTURES_MAX_LAYERS 50

typedef struct
{
    TextureLayerDefinition* layer;
    double presence;
    Color color;
} TexturesLayerResult;

typedef struct
{
    Vector3 base_location;
    Vector3 base_normal;
    int layer_count;
    TexturesLayerResult layers[TEXTURES_MAX_LAYERS];
    Vector3 final_location;
    Color final_color;
} TexturesResult;

typedef Vector3 (*FuncTexturesDisplaceTerrain)(Renderer* renderer, TerrainResult terrain);
typedef double (*FuncTexturesGetBasePresence)(Renderer* renderer, int layer, TerrainResult terrain);
typedef TexturesResult (*FuncTexturesApplyToTerrain)(Renderer* renderer, double x, double z);

class TexturesRenderer
{
public:
    TexturesDefinition* definition;

    FuncTexturesDisplaceTerrain displaceTerrain;
    FuncTexturesGetBasePresence getBasePresence;
    FuncTexturesApplyToTerrain applyToTerrain;
};


RENDERINGSHARED_EXPORT extern StandardRenderer TexturesRendererClass;

RENDERINGSHARED_EXPORT double texturesGetMaximalDisplacement(TexturesDefinition* textures);

#endif
