#ifndef _PAYSAGES_TERRAIN_H_
#define _PAYSAGES_TERRAIN_H_

#include "shared/types.h"
#include "modifiers.h"
#include "lighting.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_HEIGHT_MODIFIER_COUNT 50

typedef struct
{
    NoiseGenerator* height_noise;
    int height_modifiers_count;
    HeightModifier* height_modifiers[MAX_HEIGHT_MODIFIER_COUNT];
} TerrainDefinition;

typedef struct
{
    double min_chunk_size;
    double visible_chunk_size;
} TerrainQuality;

typedef struct
{
    int toggle_fog;
    LightingDefinition* lighting_definition;
    LightingEnvironment* lighting_environment;
} TerrainEnvironment;

void terrainInit();
void terrainSave(FILE* f);
void terrainLoad(FILE* f);

TerrainDefinition terrainCreateDefinition();
void terrainDeleteDefinition(TerrainDefinition definition);
void terrainCopyDefinition(TerrainDefinition source, TerrainDefinition* destination);
void terrainSetDefinition(TerrainDefinition definition);
TerrainDefinition terrainGetDefinition();

int terrainAddModifier(TerrainDefinition* definition, HeightModifier* modifier);
void terrainDelModifier(TerrainDefinition* definition, int modifier_position);

void terrainSetQuality(TerrainQuality quality);
TerrainQuality terrainGetQuality();

Color terrainLightFilter(Color light, Vector3 location, Vector3 light_location, Vector3 direction_to_light, void* custom_data);
int terrainProjectRay(Vector3 start, Vector3 direction, Vector3* hit_point, Color* hit_color);
double terrainGetHeightCustom(double x, double z, TerrainDefinition* definition);
double terrainGetHeight(double x, double z);
double terrainGetHeightNormalizedCustom(double x, double z, TerrainDefinition* definition);
double terrainGetHeightNormalized(double x, double z);
Color terrainGetColorCustom(double x, double z, double detail, TerrainDefinition* definition, TerrainQuality* quality, TerrainEnvironment* environment);
Color terrainGetColor(double x, double z, double detail);
void terrainRender(RenderProgressCallback callback);

#ifdef __cplusplus
}
#endif

#endif
