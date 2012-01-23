#ifndef _PAYSAGES_TEXTURES_H_
#define _PAYSAGES_TEXTURES_H_

#include "shared/types.h"
#include "noise.h"
#include "lighting.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TEXTURES_MAX_LAYERS 50

typedef struct
{
    Zone* zone;
    NoiseGenerator* bump_noise;
    Color color;
} TextureLayerDefinition;

typedef struct
{
    int nbtextures;
    TextureLayerDefinition textures[TEXTURES_MAX_LAYERS];
} TexturesDefinition;

void texturesInit();
void texturesSave(FILE* f, TexturesDefinition* definition);
void texturesLoad(FILE* f, TexturesDefinition* definition);

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

Color texturesGetLayerColor(TextureLayerDefinition* definition, Renderer* renderer, Vector3 location, double detail);
Color texturesGetColorCustom(TexturesDefinition* definition, Renderer* renderer, Vector3 location, double detail);

#ifdef __cplusplus
}
#endif

#endif
