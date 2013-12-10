#ifndef LAYERS_H
#define LAYERS_H

#include "definition_global.h"

#include "BaseDefinition.h"

namespace paysages {
namespace definition {

typedef BaseDefinition* (*LayerConstructor)(Layers* parent);

/**
 * @brief Layers of definitions, ideally all of the same type.
 */
class DEFINITIONSHARED_EXPORT Layers:public BaseDefinition
{
public:
    Layers(BaseDefinition* parent, LayerConstructor layer_constructor);
    virtual ~Layers();

    virtual void copy(BaseDefinition* destination) const override;
    Layers* newCopy() const;

    void setMaxLayerCount(int max_layer_count);

    int count() const;
    BaseDefinition* getLayer(int position) const;
    int findLayer(BaseDefinition* layer) const;

    /**
     * @brief Add a new layer
     *
     * This method takes ownership of the layer definition. In any case, it will be deleted by
     * this object (even if the layer could not be added).
     * @return The position of the new layer, -1 if it couldn't be added.
     */
    int addLayer(BaseDefinition *layer);
    int addLayer();
    void removeLayer(int position);
    void removeLayer(BaseDefinition* layer);
    void moveLayer(int old_position, int new_position);
    void moveLayer(BaseDefinition* layer, int new_position);
    void clear();

public:
    LayerConstructor layer_constructor;
    int max_layer_count;
    std::vector<BaseDefinition*> layers;
    BaseDefinition* null_layer;
};

}
}

#endif // LAYERS_H
