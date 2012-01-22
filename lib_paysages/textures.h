#ifndef _PAYSAGES_TEXTURES_H_
#define _PAYSAGES_TEXTURES_H_

#include "shared/types.h"
#include "lighting.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    Zone* zone;
    NoiseGenerator* bump_noise;
    Color color;
} TextureDefinition;

typedef struct
{
    int unused;
} TextureQuality;

typedef struct
{
    LightingDefinition* lighting_definition;
    LightingEnvironment* lighting_environment;
} TextureEnvironment;

void texturesInit();
void texturesSave(FILE* f);
void texturesLoad(FILE* f);

int texturesGetLayerCount();
int texturesAddLayer();
void texturesDeleteLayer(int layer);

TextureDefinition texturesCreateDefinition();
void texturesDeleteDefinition(TextureDefinition definition);
void texturesCopyDefinition(TextureDefinition source, TextureDefinition* destination);
void texturesSetDefinition(int layer, TextureDefinition definition);
TextureDefinition texturesGetDefinition(int layer);

void texturesSetQuality(TextureQuality quality);
TextureQuality texturesGetQuality();

Color texturesGetLayerColorCustom(Vector3 location, double detail, TextureDefinition* definition, TextureQuality* quality, TextureEnvironment* environment);
Color texturesGetColorCustom(Vector3 location, double detail, TextureQuality* quality, TextureEnvironment* environment);
Color texturesGetColor(Vector3 location);

#ifdef __cplusplus
}
#endif

#endif
