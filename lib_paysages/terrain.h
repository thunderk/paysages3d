#ifndef _PAYSAGES_TERRAIN_H_
#define _PAYSAGES_TERRAIN_H_

#include "shared/types.h"
#include "modifiers.h"
#include "noise.h"
#include "lighting.h"
#include "pack.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TERRAIN_MAX_MODIFIERS 50

typedef struct
{
    NoiseGenerator* height_noise;
    float height_factor;
    float scaling;
    int height_modifiers_count;
    HeightModifier* height_modifiers[TERRAIN_MAX_MODIFIERS];

    float _max_height;
} TerrainDefinition;

void terrainInit();
void terrainQuit();
void terrainSave(PackStream* stream, TerrainDefinition* definition);
void terrainLoad(PackStream* stream, TerrainDefinition* definition);

TerrainDefinition terrainCreateDefinition();
void terrainDeleteDefinition(TerrainDefinition* definition);
void terrainCopyDefinition(TerrainDefinition* source, TerrainDefinition* destination);
void terrainValidateDefinition(TerrainDefinition* definition);

int terrainAddModifier(TerrainDefinition* definition, HeightModifier* modifier);
void terrainDelModifier(TerrainDefinition* definition, int modifier_position);

Color terrainLightFilter(TerrainDefinition* definition, Renderer* renderer, Color light, Vector3 location, Vector3 light_location, Vector3 direction_to_light);
int terrainProjectRay(TerrainDefinition* definition, Renderer* renderer, Vector3 start, Vector3 direction, Vector3* hit_point, Color* hit_color);
float terrainGetHeight(TerrainDefinition* definition, float x, float z);
float terrainGetHeightNormalized(TerrainDefinition* definition, float x, float z);
Color terrainGetColor(TerrainDefinition* definition, Renderer* renderer, float x, float z, float detail);
void terrainRender(TerrainDefinition* definition, Renderer* renderer);

#ifdef __cplusplus
}
#endif

#endif
