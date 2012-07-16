#include "terraincanvas.h"

#include <stdlib.h>
#include <string.h>

TerrainCanvas* terrainCanvasCreate()
{
    TerrainCanvas* result = malloc(sizeof(TerrainCanvas));
    
    result->area.bounded = 1;
    result->area.location_x = -40.0;
    result->area.location_z = -40.0;
    result->area.size_x = 80.0;
    result->area.size_z = 80.0;
    result->offset_z = 0.0;
    result->height_map = heightmapCreate();
    heightmapChangeResolution(&result->height_map, 256, 256);
    result->height_factor = 1.0;
    result->detail_noise = noiseCreateGenerator();
    result->detail_height_factor = 0.1;
    result->detail_scaling = 1.0;
    result->mask_mode = TERRAINCANVAS_MASKMODE_SQUARE;
    result->mask_smoothing = 0.0;
    
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
    destination->area = source->area;
    destination->offset_z = source->offset_z;
    destination->height_factor = source->height_factor;
    heightmapCopy(&source->height_map, &destination->height_map);
    noiseCopy(source->detail_noise, destination->detail_noise);
    destination->detail_height_factor = source->detail_height_factor;
    destination->detail_scaling = source->detail_scaling;
    destination->mask_mode = source->mask_mode;
    destination->mask_smoothing = source->mask_smoothing;
}

void terrainCanvasValidate(TerrainCanvas* canvas)
{
    if (canvas->detail_scaling < 0.00001)
    {
        canvas->detail_scaling = 0.00001;
    }
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
    packWriteInt(stream, &canvas->area.bounded);
    packWriteDouble(stream, &canvas->area.location_x);
    packWriteDouble(stream, &canvas->area.location_z);
    packWriteDouble(stream, &canvas->area.size_x);
    packWriteDouble(stream, &canvas->area.size_z);
    packWriteDouble(stream, &canvas->offset_z);
    heightmapSave(stream, &canvas->height_map);
    packWriteDouble(stream, &canvas->height_factor);
    noiseSaveGenerator(stream, canvas->detail_noise);
    packWriteDouble(stream, &canvas->detail_height_factor);
    packWriteDouble(stream, &canvas->detail_scaling);
    packWriteInt(stream, &canvas->mask_mode);
    packWriteDouble(stream, &canvas->mask_smoothing);
}

void terrainCanvasLoad(PackStream* stream, TerrainCanvas* canvas)
{
    packReadInt(stream, &canvas->area.bounded);
    packReadDouble(stream, &canvas->area.location_x);
    packReadDouble(stream, &canvas->area.location_z);
    packReadDouble(stream, &canvas->area.size_x);
    packReadDouble(stream, &canvas->area.size_z);
    packReadDouble(stream, &canvas->offset_z);
    heightmapLoad(stream, &canvas->height_map);
    packReadDouble(stream, &canvas->height_factor);
    noiseLoadGenerator(stream, canvas->detail_noise);
    packReadDouble(stream, &canvas->detail_height_factor);
    packReadDouble(stream, &canvas->detail_scaling);
    packReadInt(stream, &canvas->mask_mode);
    packReadDouble(stream, &canvas->mask_smoothing);
}

void terrainCanvasRevertToTerrain(TerrainCanvas* canvas, TerrainDefinition* terrain, int only_masked)
{
}

Vector3 terrainCanvasApply(TerrainCanvas* canvas, Vector3 location)
{
    if (location.x >= canvas->area.location_x &&
        location.z >= canvas->area.location_z &&
        location.x <= canvas->area.location_x + canvas->area.size_x &&
        location.z <= canvas->area.location_z + canvas->area.size_z)
    {
        location.y = heightmapGetValue(&canvas->height_map, (location.x - canvas->area.location_x) / canvas->area.size_x, (location.z - canvas->area.location_z) / canvas->area.size_z);
    }
    return location;
}
