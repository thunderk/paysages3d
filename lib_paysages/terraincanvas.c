#include "terraincanvas.h"

#include <stdlib.h>
#include <string.h>

TerrainCanvas* terrainCanvasCreate()
{
    TerrainCanvas* result = malloc(sizeof(TerrainCanvas));
    
    result->area.bounded = 1;
    result->area.location_x = 0.0;
    result->area.location_z = 0.0;
    result->area.size_x = 1.0;
    result->area.size_z = 1.0;
    result->offset_z = 0.0;
    result->height_map.data = malloc(sizeof(double));
    result->height_map.resolution_x = 1;
    result->height_map.resolution_z = 1;
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
    free(canvas->height_map.data);
    noiseDeleteGenerator(canvas->detail_noise);
    free(canvas);
}

void terrainCanvasCopy(TerrainCanvas* source, TerrainCanvas* destination)
{
    destination->area = source->area;
    destination->offset_z = source->offset_z;
    destination->height_map.resolution_x = source->height_map.resolution_x;
    destination->height_map.resolution_z = source->height_map.resolution_z;
    destination->height_map.data = realloc(destination->height_map.data, sizeof(double) * destination->height_map.resolution_x * destination->height_map.resolution_z);
    memcpy(destination->height_map.data, source->height_map.data, sizeof(double) * destination->height_map.resolution_x * destination->height_map.resolution_z);
    destination->height_factor = source->height_factor;
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
    int i;
    
    packWriteInt(stream, &canvas->area.bounded);
    packWriteDouble(stream, &canvas->area.location_x);
    packWriteDouble(stream, &canvas->area.location_z);
    packWriteDouble(stream, &canvas->area.size_x);
    packWriteDouble(stream, &canvas->area.size_z);
    packWriteDouble(stream, &canvas->offset_z);
    packWriteInt(stream, &canvas->height_map.resolution_x);
    packWriteInt(stream, &canvas->height_map.resolution_z);
    for (i = 0; i < canvas->height_map.resolution_x * canvas->height_map.resolution_z; i++)
    {
        packWriteDouble(stream, &canvas->height_map.data[i]);
    }
    packWriteDouble(stream, &canvas->height_factor);
    noiseSaveGenerator(stream, canvas->detail_noise);
    packWriteDouble(stream, &canvas->detail_height_factor);
    packWriteDouble(stream, &canvas->detail_scaling);
    packWriteInt(stream, &canvas->mask_mode);
    packWriteDouble(stream, &canvas->mask_smoothing);
}

void terrainCanvasLoad(PackStream* stream, TerrainCanvas* canvas)
{
    int i;
    
    packReadInt(stream, &canvas->area.bounded);
    packReadDouble(stream, &canvas->area.location_x);
    packReadDouble(stream, &canvas->area.location_z);
    packReadDouble(stream, &canvas->area.size_x);
    packReadDouble(stream, &canvas->area.size_z);
    packReadDouble(stream, &canvas->offset_z);
    packReadInt(stream, &canvas->height_map.resolution_x);
    packReadInt(stream, &canvas->height_map.resolution_z);
    canvas->height_map.data = realloc(canvas->height_map.data, sizeof(double) * canvas->height_map.resolution_x * canvas->height_map.resolution_z);
    for (i = 0; i < canvas->height_map.resolution_x * canvas->height_map.resolution_z; i++)
    {
        packReadDouble(stream, &canvas->height_map.data[i]);
    }
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

Vector3 terrainCanvasApply(TerrainCanvas* canvas, Vector3 position)
{
}
