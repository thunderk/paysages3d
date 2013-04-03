#ifndef _PAYSAGES_TEXTURES_PUBLIC_H_
#define _PAYSAGES_TEXTURES_PUBLIC_H_

#include "../layers.h"
#include "../zone.h"
#include "../tools/lighting.h"
#include "../terrain/public.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TEXTURES_MAX_LAYERS 50

typedef enum
{
    TEXTURES_PRESET_IRELAND,
    TEXTURES_PRESET_ALPS,
    TEXTURES_PRESET_CANYON
} TexturesPreset;

typedef enum
{
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
    /*NoiseGenerator* _detail_noise;*/
    /*Curve* _local_slope_condition;*/
} TexturesLayerDefinition;

typedef struct
{
    Layers* layers;
} TexturesDefinition;

typedef struct
{
    double global_presence;
    double local_presence;
    Vector3 displaced_location;
    Vector3 displaced_normal;
    Color color;
    Color cancelled_color;
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
typedef TexturesResult (*FuncTexturesApplyToTerrain)(Renderer* renderer, double x, double z);

typedef struct
{
    TexturesDefinition* definition;

    FuncTexturesDisplaceTerrain displaceTerrain;
    FuncTexturesApplyToTerrain applyToTerrain;
} TexturesRenderer;


extern StandardDefinition TexturesDefinitionClass;
extern StandardRenderer TexturesRendererClass;


LayerType texturesGetLayerType();
void texturesAutoPreset(TexturesDefinition* definition, TexturesPreset preset);
void texturesLayerAutoPreset(TexturesLayerDefinition* definition, TexturesLayerPreset preset);

/*Renderer* cloudsCreatePreviewCoverageRenderer();
Color cloudsGetPreviewCoverage(Renderer* renderer, double x, double y, double scaling, int perspective);

Renderer* cloudsCreatePreviewColorRenderer();
Color cloudsGetPreviewColor(Renderer* renderer, double x, double y);*/

#ifdef __cplusplus
}
#endif

#endif
