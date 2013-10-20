#ifndef _PAYSAGES_LAYERS_H_
#define _PAYSAGES_LAYERS_H_

/* Factorized layer management (with names) */

#include "rendering_global.h"
#include "tools/pack.h"

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

RENDERINGSHARED_EXPORT Layers* layersCreate(LayerType type, int max_layer_count);
RENDERINGSHARED_EXPORT Layers* layersCreateCopy(Layers* original);
RENDERINGSHARED_EXPORT void layersDelete(Layers* layers);

RENDERINGSHARED_EXPORT void layersCopy(Layers* source, Layers* destination);
RENDERINGSHARED_EXPORT void layersValidate(Layers* layers);

RENDERINGSHARED_EXPORT void layersSave(PackStream* stream, Layers* layers);
RENDERINGSHARED_EXPORT void layersLoad(PackStream* stream, Layers* layers);

RENDERINGSHARED_EXPORT const char* layersGetName(Layers* layers, int layer);
RENDERINGSHARED_EXPORT void layersSetName(Layers* layers, int layer, const char* name);

RENDERINGSHARED_EXPORT void layersClear(Layers* layers);
RENDERINGSHARED_EXPORT int layersCount(Layers* layers);
RENDERINGSHARED_EXPORT void* layersGetLayer(Layers* layers, int layer);
RENDERINGSHARED_EXPORT int layersAddLayer(Layers* layers, void* definition);
RENDERINGSHARED_EXPORT void layersDeleteLayer(Layers* layers, int layer);
RENDERINGSHARED_EXPORT void layersMove(Layers* layers, int layer, int new_position);

#ifdef __cplusplus
}
#endif

#endif
