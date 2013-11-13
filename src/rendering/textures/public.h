#ifndef _PAYSAGES_TEXTURES_PUBLIC_H_
#define _PAYSAGES_TEXTURES_PUBLIC_H_

#include "../rendering_global.h"
#include "Layers.h"
#include "tools/zone.h"
#include "tools/lighting.h"
#include "terrain/public.h"
#include "SurfaceMaterial.h"

#define TEXTURES_MAX_LAYERS 50

typedef enum
{
    TEXTURES_PRESET_FULL,
    TEXTURES_PRESET_IRELAND,
    TEXTURES_PRESET_ALPS,
    TEXTURES_PRESET_CANYON
} TexturesPreset;

typedef enum
{
    TEXTURES_LAYER_PRESET_MUD,
    TEXTURES_LAYER_PRESET_ROCK,
    TEXTURES_LAYER_PRESET_GRASS,
    TEXTURES_LAYER_PRESET_SAND,
    TEXTURES_LAYER_PRESET_SNOW
} TexturesLayerPreset;

typedef enum
{
    TEXTURES_MERGE_FADE,
    TEXTURES_MERGE_DISSOLVE,
    TEXTURES_MERGE_DISPLACEMENT_VALUE
} TexturesMergeMode;

typedef struct
{
    Zone* terrain_zone;
    double displacement_scaling;
    double displacement_height;
    double displacement_offset;
    /*double detail_scaling;
    double detail_height;*/
    SurfaceMaterial material;
    /*double cancel_displacement_factor;
    TexturesMergeMode merge_mode;*/

    NoiseGenerator* _displacement_noise;
    NoiseGenerator* _detail_noise;
    /*Curve* _local_slope_condition;*/
} TexturesLayerDefinition;

class TexturesDefinition
{
public:
    Layers* layers;
};

typedef struct
{
    TexturesLayerDefinition* layer;
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


RENDERINGSHARED_EXPORT extern StandardDefinition TexturesDefinitionClass;
RENDERINGSHARED_EXPORT extern StandardRenderer TexturesRendererClass;


RENDERINGSHARED_EXPORT LayerType texturesGetLayerType();
RENDERINGSHARED_EXPORT void texturesAutoPreset(TexturesDefinition* definition, TexturesPreset preset);
RENDERINGSHARED_EXPORT void texturesLayerAutoPreset(TexturesLayerDefinition* definition, TexturesLayerPreset preset);

RENDERINGSHARED_EXPORT double texturesGetMaximalDisplacement(TexturesDefinition* textures);

#endif
