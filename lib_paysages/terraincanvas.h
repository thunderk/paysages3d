#ifndef _PAYSAGES_TERRAINCANVAS_H_
#define _PAYSAGES_TERRAINCANVAS_H_

/* Terrain edition by painting over an area */

#include "pack.h"
#include "noise.h"
#include "terrain.h"
#include "layers.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    int bounded;
    double location_x;
    double location_z;
    double size_x;
    double size_z;
} GeoArea;

typedef struct
{
    int resolution_x;
    int resolution_z;
    double* data;
} HeightMap;

#define TERRAINCANVAS_MASKMODE_SQUARE 0
#define TERRAINCANVAS_MASKMODE_CIRCLE 1

typedef struct
{
    GeoArea area;
    double offset_z;
    HeightMap height_map;
    double height_factor;
    NoiseGenerator* detail_noise;
    double detail_height_factor;
    double detail_scaling;
    int mask_mode;
    double mask_smoothing;
} TerrainCanvas;

TerrainCanvas* terrainCanvasCreate();
void terrainCanvasDelete(TerrainCanvas* canvas);
void terrainCanvasCopy(TerrainCanvas* source, TerrainCanvas* destination);
void terrainCanvasValidate(TerrainCanvas* canvas);
LayerType terrainCanvasGetLayerType();

void terrainCanvasSave(PackStream* stream, TerrainCanvas* canvas);
void terrainCanvasLoad(PackStream* stream, TerrainCanvas* canvas);

void terrainCanvasRevertToTerrain(TerrainCanvas* canvas, TerrainDefinition* terrain, int only_masked);
Vector3 terrainCanvasApply(TerrainCanvas* canvas, Vector3 position);
    
#ifdef __cplusplus
}
#endif

#endif
