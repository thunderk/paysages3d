#ifndef _PAYSAGES_TEXTURES_H_
#define _PAYSAGES_TEXTURES_H_

#include "shared/types.h"
#include "noise.h"
#include "lighting.h"
#include "pack.h"
#include "zone.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TEXTURES_MAX_LAYERS 50

typedef struct
{
    Zone* zone;
    NoiseGenerator* bump_noise;
    double bump_scaling;
    double bump_height;
    SurfaceMaterial material;
} TextureLayerDefinition;

typedef struct
{
    int nbtextures;
    TextureLayerDefinition textures[TEXTURES_MAX_LAYERS];
} TexturesDefinition;

void texturesInit();
void texturesQuit();
void texturesSave(PackStream* stream, TexturesDefinition* definition);
void texturesLoad(PackStream* stream, TexturesDefinition* definition);

TexturesDefinition texturesCreateDefinition();
void texturesDeleteDefinition(TexturesDefinition* definition);
void texturesCopyDefinition(TexturesDefinition* source, TexturesDefinition* destination);
void texturesValidateDefinition(TexturesDefinition* definition);

TextureLayerDefinition texturesLayerCreateDefinition();
void texturesLayerDeleteDefinition(TextureLayerDefinition* definition);
void texturesLayerCopyDefinition(TextureLayerDefinition* source, TextureLayerDefinition* destination);
void texturesLayerValidateDefinition(TextureLayerDefinition* definition);

int texturesGetLayerCount(TexturesDefinition* definition);
TextureLayerDefinition* texturesGetLayer(TexturesDefinition* definition, int layer);
int texturesAddLayer(TexturesDefinition* definition);
void texturesDeleteLayer(TexturesDefinition* definition, int layer);

double texturesGetLayerCoverage(TextureLayerDefinition* definition, Renderer* renderer, Vector3 location, double detail);
Color texturesGetLayerColor(TextureLayerDefinition* definition, Renderer* renderer, Vector3 location, double detail);
Color texturesGetColor(TexturesDefinition* definition, Renderer* renderer, Vector3 location, double detail);

#ifdef __cplusplus
}
#endif

#endif
