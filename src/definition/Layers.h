#ifndef LAYERS_H
#define LAYERS_H

#include "definition_global.h"
#include "BaseDefinition.h"
#include "LegacyLayer.h"

namespace paysages {
namespace system {class PackStream;}
namespace definition {

class Layers;

typedef BaseDefinition* (*LayerConstructor)(Layers* parent);

/**
 * @brief Layers of definitions, ideally all of the same type.
 */
class DEFINITIONSHARED_EXPORT Layers:public BaseDefinition
{
public:
    Layers(BaseDefinition* parent, LayerConstructor layer_constructor, LayerType* legacy_type=0);
    virtual ~Layers();

    virtual void copy(BaseDefinition* destination);

    void setMaxLayerCount(int max_layer_count);

    int count();
    BaseDefinition* getLayer(int position);
    int findLayer(BaseDefinition* layer);

    /**
     * @brief Add a new layer
     *
     * This method takes ownership of the layer definition. In any case, it will be deleted by
     * this object (even if the layer could not be added).
     * @return The position of the new layer, -1 if it couldn't be added.
     */
    int addLayer(BaseDefinition* layer);
    int addLayer();
    void removeLayer(int position);
    void removeLayer(BaseDefinition* layer);
    void moveLayer(int old_position, int new_position);
    void moveLayer(BaseDefinition* layer, int new_position);
    void clear();

    // Transitional data storage
    LayerType legacy_type;

private:
    LayerConstructor layer_constructor;
    int max_layer_count;
    QList<BaseDefinition*> layers;
    BaseDefinition* null_layer;
};

}
}

DEFINITIONSHARED_EXPORT Layers* layersCreate(LayerType type, int max_layer_count);
DEFINITIONSHARED_EXPORT Layers* layersCreateCopy(Layers* original);
DEFINITIONSHARED_EXPORT void layersDelete(Layers* layers);

DEFINITIONSHARED_EXPORT void layersCopy(Layers* source, Layers* destination);
DEFINITIONSHARED_EXPORT void layersValidate(Layers* layers);

DEFINITIONSHARED_EXPORT void layersSave(PackStream* stream, Layers* layers);
DEFINITIONSHARED_EXPORT void layersLoad(PackStream* stream, Layers* layers);

DEFINITIONSHARED_EXPORT const char* layersGetName(Layers* layers, int layer);
DEFINITIONSHARED_EXPORT void layersSetName(Layers* layers, int layer, const char* name);

DEFINITIONSHARED_EXPORT void layersClear(Layers* layers);
DEFINITIONSHARED_EXPORT int layersCount(Layers* layers);
DEFINITIONSHARED_EXPORT void* layersGetLayer(Layers* layers, int layer);
DEFINITIONSHARED_EXPORT int layersAddLayer(Layers* layers, void* definition);
DEFINITIONSHARED_EXPORT void layersDeleteLayer(Layers* layers, int layer);
DEFINITIONSHARED_EXPORT void layersMove(Layers* layers, int layer, int new_position);

#endif // LAYERS_H
