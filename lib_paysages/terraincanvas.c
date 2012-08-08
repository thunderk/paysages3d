#include "terraincanvas.h"
#include "scenery.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

TerrainCanvas* terrainCanvasCreate()
{
    TerrainCanvas* result = malloc(sizeof(TerrainCanvas));
    
    result->area = geoareaCreate();
    result->offset_y = 0.0;
    result->height_map = heightmapCreate();
    heightmapChangeResolution(&result->height_map, 256, 256);
    result->height_factor = 1.0;
    result->detail_noise = noiseCreateGenerator();
    noiseGenerateBaseNoise(result->detail_noise, 1048576);
    noiseAddLevelsSimple(result->detail_noise, 5, 1.0, 1.0);
    result->detail_height_factor = 0.2;
    result->detail_scaling = 0.4;
    result->mask.mode = INTEGRATIONMASK_MODE_CIRCLE;
    result->mask.smoothing = 0.1;
    
    return result;
}

void terrainCanvasDelete(TerrainCanvas* canvas)
{
    heightmapDelete(&canvas->height_map);
    noiseDeleteGenerator(canvas->detail_noise);
    free(canvas);
}

void terrainCanvasCopy(TerrainCanvas* source, TerrainCanvas* destination)
{
    geoareaCopy(&source->area, &destination->area);
    destination->offset_y = source->offset_y;
    destination->height_factor = source->height_factor;
    heightmapCopy(&source->height_map, &destination->height_map);
    noiseCopy(source->detail_noise, destination->detail_noise);
    destination->detail_height_factor = source->detail_height_factor;
    destination->detail_scaling = source->detail_scaling;
    destination->mask = source->mask;
}

void terrainCanvasValidate(TerrainCanvas* canvas)
{
    if (canvas->detail_scaling < 0.00001)
    {
        canvas->detail_scaling = 0.00001;
    }
    geoareaValidate(&canvas->area);
    heightmapValidate(&canvas->height_map);
    noiseValidate(canvas->detail_noise);
}

LayerType terrainCanvasGetLayerType()
{
    LayerType result;
    
    result.callback_create = (LayerCallbackCreate)terrainCanvasCreate;
    result.callback_delete = (LayerCallbackDelete)terrainCanvasDelete;
    result.callback_copy = (LayerCallbackCopy)terrainCanvasCopy;
    result.callback_validate = (LayerCallbackValidate)terrainCanvasValidate;
    result.callback_save = (LayerCallbackSave)terrainCanvasSave;
    result.callback_load = (LayerCallbackLoad)terrainCanvasLoad;
    
    return result;
}

void terrainCanvasSave(PackStream* stream, TerrainCanvas* canvas)
{
    geoareaSave(stream, &canvas->area);
    packWriteDouble(stream, &canvas->offset_y);
    heightmapSave(stream, &canvas->height_map);
    packWriteDouble(stream, &canvas->height_factor);
    noiseSaveGenerator(stream, canvas->detail_noise);
    packWriteDouble(stream, &canvas->detail_height_factor);
    packWriteDouble(stream, &canvas->detail_scaling);
    packWriteInt(stream, &canvas->mask.mode);
    packWriteDouble(stream, &canvas->mask.smoothing);
}

void terrainCanvasLoad(PackStream* stream, TerrainCanvas* canvas)
{
    geoareaLoad(stream, &canvas->area);
    packReadDouble(stream, &canvas->offset_y);
    heightmapLoad(stream, &canvas->height_map);
    packReadDouble(stream, &canvas->height_factor);
    noiseLoadGenerator(stream, canvas->detail_noise);
    packReadDouble(stream, &canvas->detail_height_factor);
    packReadDouble(stream, &canvas->detail_scaling);
    packReadInt(stream, &canvas->mask.mode);
    packReadDouble(stream, &canvas->mask.smoothing);
}

void terrainCanvasGetLimits(TerrainCanvas* canvas, double* ymin, double* ymax)
{
    double noise_max = noiseGetMaxValue(canvas->detail_noise) * canvas->detail_height_factor;
    heightmapGetLimits(&canvas->height_map, ymin, ymax);
    *ymin -= noise_max;
    *ymax += noise_max;
}

void terrainCanvasRevertToTerrain(TerrainCanvas* canvas)
{
    TerrainDefinition terrain;
    
    terrain = terrainCreateDefinition();
    sceneryGetTerrain(&terrain);
    
    heightmapRevertToTerrain(&canvas->height_map, &terrain, &canvas->area);
    
    terrainDeleteDefinition(&terrain);
}

Vector3 terrainCanvasApply(TerrainCanvas* canvas, Vector3 location)
{
    if (location.x >= canvas->area.location_x &&
        location.z >= canvas->area.location_z &&
        location.x <= canvas->area.location_x + canvas->area.size_x &&
        location.z <= canvas->area.location_z + canvas->area.size_z)
    {
        double inside_x, inside_z;
        double height, distance;
        
        /* Get height map displacement */
        geoareaToLocal(&canvas->area, location.x, location.z, &inside_x, &inside_z);
        height = heightmapGetValue(&canvas->height_map, inside_x, inside_z);
        
        /* Apply factor */
        height = height * canvas->height_factor + canvas->offset_y;
        
        /* Apply detail noise */
        height += noiseGet2DTotal(canvas->detail_noise, location.x / canvas->detail_scaling, location.z / canvas->detail_scaling) * canvas->detail_height_factor;
        
        /* Apply integration mask */
        double influence = terrainCanvasGetMaskValue(canvas, inside_x, inside_z);
        location.y = influence * height + (1.0 - influence) * location.y;
        
    }
    return location;
}

double terrainCanvasGetMaskValue(TerrainCanvas* canvas, double local_x, double local_z)
{
    double distance;
    
    local_x = (local_x - 0.5) * 2.0;
    local_z = (local_z - 0.5) * 2.0;
    
    if (canvas->mask.mode == INTEGRATIONMASK_MODE_SQUARE)
    {
        local_x = fabs(local_x);
        local_z = fabs(local_z);
        distance = local_x > local_z ? local_x : local_z;
    }
    else
    {
        distance = sqrt(local_x * local_x + local_z * local_z);
    }
    
    if (distance <= 1.0 - canvas->mask.smoothing)
    {
        return 1.0;
    }
    else if (distance <= 1.0)
    {
        return (1.0 - distance) / canvas->mask.smoothing;
    }
    else
    {
        return 0.0;
    }
}
