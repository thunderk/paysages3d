#ifndef _PAYSAGES_TERRAIN_H_
#define _PAYSAGES_TERRAIN_H_

#include "shared/types.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    NoiseGenerator* height_noise;
} TerrainDefinition;

typedef struct
{
    double min_chunk_size;
    double visible_chunk_size;
} TerrainQuality;

typedef struct
{
    int unused;
} TerrainEnvironment;

void terrainInit();
void terrainSave(FILE* f);
void terrainLoad(FILE* f);

TerrainDefinition terrainCreateDefinition();
void terrainDeleteDefinition(TerrainDefinition definition);
void terrainCopyDefinition(TerrainDefinition source, TerrainDefinition* destination);
void terrainSetDefinition(TerrainDefinition definition);
TerrainDefinition terrainGetDefinition();

void terrainSetQuality(TerrainQuality quality);
TerrainQuality terrainGetQuality();

double terrainGetShadow(Vector3 start, Vector3 direction);
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
