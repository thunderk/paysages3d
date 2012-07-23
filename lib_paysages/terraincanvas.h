#ifndef _PAYSAGES_TERRAINCANVAS_H_
#define _PAYSAGES_TERRAINCANVAS_H_

/* Terrain edition by painting over an area */

#include "pack.h"
#include "noise.h"
#include "terrain.h"
#include "layers.h"
#include "heightmap.h"

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
    int mode;
    double smoothing;
} IntegrationMask;

#define INTEGRATIONMASK_MODE_SQUARE 0
#define INTEGRATIONMASK_MODE_CIRCLE 1

typedef struct
{
    GeoArea area;
    double offset_y;
    HeightMap height_map;
    double height_factor;
    NoiseGenerator* detail_noise;
    double detail_height_factor;
    double detail_scaling;
    IntegrationMask mask;
} TerrainCanvas;

TerrainCanvas* terrainCanvasCreate();
void terrainCanvasDelete(TerrainCanvas* canvas);
void terrainCanvasCopy(TerrainCanvas* source, TerrainCanvas* destination);
void terrainCanvasValidate(TerrainCanvas* canvas);
LayerType terrainCanvasGetLayerType();

void terrainCanvasSave(PackStream* stream, TerrainCanvas* canvas);
void terrainCanvasLoad(PackStream* stream, TerrainCanvas* canvas);

double terrainCanvasGetLimits(TerrainCanvas* canvas, double* ymin, double* ymax);
void terrainCanvasRevertToTerrain(TerrainCanvas* canvas, TerrainDefinition* terrain, int only_masked);
Vector3 terrainCanvasApply(TerrainCanvas* canvas, Vector3 location);
    
#ifdef __cplusplus
}
#endif

#endif
