#ifndef LAYERS_H
#define LAYERS_H

#include "definition_global.h"

#include "DefinitionNode.h"

namespace paysages {
namespace definition {

typedef DefinitionNode* (*LayerConstructor)(Layers* parent);

/**
 * @brief Layers of definitions, ideally all of the same type.
 */
class DEFINITIONSHARED_EXPORT Layers:public DefinitionNode
{
public:
    Layers(DefinitionNode* parent, const std::string &name, LayerConstructor layer_constructor);
    virtual ~Layers();

    virtual void save(PackStream *stream) const override;
    virtual void load(PackStream *stream) override;
    virtual void copy(DefinitionNode* destination) const override;
    Layers* newCopy() const;

    void setMaxLayerCount(int max_layer_count);

    int count() const;
    DefinitionNode* getLayer(int position) const;
    int findLayer(DefinitionNode* layer) const;

    /**
     * @brief Add a new layer
     *
     * This method takes ownership of the layer definition. In any case, it will be deleted by
     * this object (even if the layer could not be added).
     * @return The position of the new layer, -1 if it couldn't be added.
     */
    int addLayer(DefinitionNode *layer);
    int addLayer();
    void removeLayer(int position);
    void removeLayer(DefinitionNode* layer);
    void moveLayer(int old_position, int new_position);
    void moveLayer(DefinitionNode* layer, int new_position);
    void clear();

protected:
    virtual DefinitionNode *findChildByName(const std::string name) override;

public:
    LayerConstructor layer_constructor;
    int max_layer_count;
    std::vector<DefinitionNode*> layers;
    DefinitionNode* null_layer;

};

}
}

#endif // LAYERS_H
