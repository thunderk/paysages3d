#ifndef _PAYSAGES_LAYERS_H_
#define _PAYSAGES_LAYERS_H_

/* Factorized layer management (with names) */

#include "pack.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* (*LayerCallbackCreate)();
typedef void (*LayerCallbackDelete)(void* layer);
typedef void (*LayerCallbackCopy)(void* source, void* definition);
typedef void (*LayerCallbackValidate)(void* layer);
typedef void (*LayerCallbackSave)(PackStream* stream, void* layer);
typedef void (*LayerCallbackLoad)(PackStream* stream, void* layer);
    
typedef struct {
    LayerCallbackCreate callback_create;
    LayerCallbackDelete callback_delete;
    LayerCallbackCopy callback_copy;
    LayerCallbackValidate callback_validate;
    LayerCallbackSave callback_save;
    LayerCallbackLoad callback_load;
} LayerType;

typedef struct Layers Layers;

Layers* layersCreate(LayerType type, int max_layer_count);
Layers* layersCreateCopy(Layers* original);
void layersDelete(Layers* layers);

void layersCopy(Layers* source, Layers* destination);
void layersValidate(Layers* layers);

void layersSave(PackStream* stream, Layers* layers);
void layersLoad(PackStream* stream, Layers* layers);

const char* layersGetName(Layers* layers, int layer);
void layersSetName(Layers* layers, int layer, const char* name);

int layersCount(Layers* layers);
void* layersGetLayer(Layers* layers, int layer);
int layersAddLayer(Layers* layers, void* definition);
void layersDeleteLayer(Layers* layers, int layer);
void layersMove(Layers* layers, int layer, int new_position);

#ifdef __cplusplus
}
#endif

#endif
