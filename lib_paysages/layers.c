#include "layers.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define LAYERS_MAX_NAME_LENGTH 50

typedef struct
{
    void* definition;
    char name[LAYERS_MAX_NAME_LENGTH + 1];
} LayerInfo;

struct Layers {
    LayerType type;
    int count;
    int max_count;
    void* null_layer;
    LayerInfo* layers_info;
};

Layers* layersCreate(LayerType type, int max_layer_count)
{
    Layers* result = malloc(sizeof(Layers));
    
    result->type = type;
    result->count = 0;
    result->max_count = max_layer_count;
    result->null_layer = type.callback_create();
    result->layers_info = malloc(sizeof(LayerInfo) * max_layer_count);
    
    return result;
}

void layersDelete(Layers* layers)
{
    int i;
    for (i = 0; i < layers->count; i++)
    {
        layers->type.callback_delete(layers->layers_info[i].definition);
    }
    
    free(layers->null_layer);
    free(layers->layers_info);
    free(layers);
}

void layersCopy(Layers* source, Layers* destination)
{
    int i;
    
    assert(source->type == destination->type);
    assert(source->max_count == destination->max_count);

    /* TODO Optimize by reusing common layers */
    while (destination->count > 0)
    {
        layersDeleteLayer(destination, 0);
    }
    for (i = 0; i < source->count; i++)
    {
        layersAddLayer(destination, source->layers_info[i].definition);
        layersSetName(destination, i, layersGetName(source, i));
    }
}

void layersValidate(Layers* layers)
{
    int i;
    
    for (i = 0; i < layers->count; i++)
    {
        layers->type.callback_validate(layers->layers_info[i].definition);
        layers->layers_info[i].name[LAYERS_MAX_NAME_LENGTH] = '\0';
    }
}

void layersSave(PackStream* stream, Layers* layers)
{
    int i;
    
    packWriteInt(stream, &layers->count);
    for (i = 0; i < layers->count; i++)
    {
        packWriteString(stream, layers->layers_info[i].name, LAYERS_MAX_NAME_LENGTH);
        layers->type.callback_save(stream, layers->layers_info[i].definition);
    }
}

void layersLoad(PackStream* stream, Layers* layers)
{
    int i;
    
    packReadInt(stream, &layers->count);
    for (i = 0; i < layers->count; i++)
    {
        packReadString(stream, layers->layers_info[i].name, LAYERS_MAX_NAME_LENGTH);
        layers->type.callback_load(stream, layers->layers_info[i].definition);
    }
    layersValidate(layers);
}

const char* layersGetName(Layers* layers, int layer)
{
    if (layer >= 0 && layer < layers->count)
    {
        return layers->layers_info[layer].name;
    }
    else
    {
        return "";
    }
}

void layersSetName(Layers* layers, int layer, const char* name)
{
    if (layer >= 0 && layer < layers->count)
    {
        strncpy(layers->layers_info[layer].name, name, LAYERS_MAX_NAME_LENGTH);
    }
}

int layersCount(Layers* layers)
{
    return layers->count;
}

void* layersGetLayer(Layers* layers, int layer)
{
    if (layer >= 0 && layer < layers->count)
    {
        return layers->layers_info[layer].definition;
    }
    else
    {
        return layers->null_layer;
    }
}

int layersAddLayer(Layers* layers, void* definition)
{
    if (layers->count < layers->max_count)
    {
        layers->layers_info[layers->count].definition = layers->type.callback_create();
        if (definition)
        {
            layers->type.callback_copy(definition, layers->layers_info[layers->count].definition);
        }
        
        layers->count++;
        layersSetName(layers, layers->count - 1, "unnamed");
    }
    else
    {
        return -1;
    }
}

void layersDeleteLayer(Layers* layers, int layer)
{
    if (layer >= 0 && layer < layers->count)
    {
        layers->type.callback_delete(layers->layers_info[layer].definition);
        if (layers->count > 1 && layer < layers->count - 1)
        {
            memmove(layers->layers_info + layer, layers->layers_info + layer + 1, sizeof(LayerInfo) * (layers->count - layer - 1));
        }
        layers->count--;
    }
}

void layersMove(Layers* layers, int layer, int new_position)
{
    if (layer >= 0 && layer < layers->count && new_position != layer && new_position >= 0 && new_position < layers->count)
    {
        LayerInfo temp;
        temp = layers->layers_info[layer];
        if (new_position > layer)
        {
            memmove(layers->layers_info + layer, layers->layers_info + layer + 1, sizeof(LayerInfo) * (new_position - layer));
        }
        else
        {
            memmove(layers->layers_info + new_position + 1, layers->layers_info + new_position, sizeof(LayerInfo) * (layer - new_position));
        }
        layers->layers_info[new_position] = temp;
    }
}
