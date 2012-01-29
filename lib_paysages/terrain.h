#ifndef _PAYSAGES_TERRAIN_H_
#define _PAYSAGES_TERRAIN_H_

#include "shared/types.h"
#include "modifiers.h"
#include "noise.h"
#include "lighting.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TERRAIN_MAX_MODIFIERS 50

typedef struct
{
    NoiseGenerator* height_noise;
    double height_factor;
    double scaling;
    int height_modifiers_count;
    HeightModifier* height_modifiers[TERRAIN_MAX_MODIFIERS];

    double _max_height;
} TerrainDefinition;

void terrainInit();
void terrainSave(FILE* f, TerrainDefinition* definition);
void terrainLoad(FILE* f, TerrainDefinition* definition);

TerrainDefinition terrainCreateDefinition();
void terrainDeleteDefinition(TerrainDefinition* definition);
void terrainCopyDefinition(TerrainDefinition* source, TerrainDefinition* destination);
void terrainValidateDefinition(TerrainDefinition* definition);

int terrainAddModifier(TerrainDefinition* definition, HeightModifier* modifier);
void terrainDelModifier(TerrainDefinition* definition, int modifier_position);

Color terrainLightFilter(TerrainDefinition* definition, Renderer* renderer, Color light, Vector3 location, Vector3 light_location, Vector3 direction_to_light);
int terrainProjectRay(TerrainDefinition* definition, Renderer* renderer, Vector3 start, Vector3 direction, Vector3* hit_point, Color* hit_color);
double terrainGetHeight(TerrainDefinition* definition, double x, double z);
double terrainGetHeightNormalized(TerrainDefinition* definition, double x, double z);
Color terrainGetColor(TerrainDefinition* definition, Renderer* renderer, double x, double z, double detail);
void terrainRender(TerrainDefinition* definition, Renderer* renderer);

#ifdef __cplusplus
}
#endif

#endif
