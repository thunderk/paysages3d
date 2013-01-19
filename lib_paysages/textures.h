#ifndef _PAYSAGES_TEXTURES_H_
#define _PAYSAGES_TEXTURES_H_

#include "tools/lighting.h"
#include "tools/color.h"
#include "tools/euclid.h"
#include "tools/pack.h"
#include "shared/types.h"
#include "layers.h"
#include "noise.h"
#include "zone.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    Zone* zone;
    NoiseGenerator* bump_noise;
    double bump_scaling;
    double bump_height;
    SurfaceMaterial material;
    double thickness;
    double slope_range;
    double thickness_transparency;
} TextureLayerDefinition;

typedef struct
{
    Layers* layers;
} TexturesDefinition;

TexturesDefinition texturesCreateDefinition();
void texturesDeleteDefinition(TexturesDefinition* definition);
void texturesCopyDefinition(TexturesDefinition* source, TexturesDefinition* destination);
void texturesValidateDefinition(TexturesDefinition* definition);

void texturesSave(PackStream* stream, TexturesDefinition* definition);
void texturesLoad(PackStream* stream, TexturesDefinition* definition);

TextureLayerDefinition* texturesLayerCreateDefinition();
void texturesLayerDeleteDefinition(TextureLayerDefinition* definition);
void texturesLayerCopyDefinition(TextureLayerDefinition* source, TextureLayerDefinition* destination);
void texturesLayerValidateDefinition(TextureLayerDefinition* definition);
LayerType texturesGetLayerType();

double texturesGetLayerCoverage(TextureLayerDefinition* definition, Renderer* renderer, Vector3 location, double detail);
Color texturesGetLayerColor(TextureLayerDefinition* definition, Renderer* renderer, Vector3 location, double detail);
Color texturesGetColor(TexturesDefinition* definition, Renderer* renderer, double x, double z, double detail);

#ifdef __cplusplus
}
#endif

#endif
