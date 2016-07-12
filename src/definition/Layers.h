#pragma once

#include "definition_global.h"

#include "DefinitionNode.h"

namespace paysages {
namespace definition {

typedef DefinitionNode *(*LayerConstructor)(Layers *parent, const string &name);

/**
 * Layers of definitions, ideally all of the same type.
 */
class DEFINITIONSHARED_EXPORT Layers : public DefinitionNode {
  public:
    Layers(DefinitionNode *parent, const string &name, LayerConstructor layer_constructor);
    virtual ~Layers();

    virtual void save(PackStream *stream) const override;
    virtual void load(PackStream *stream) override;
    virtual void copy(DefinitionNode *destination) const override;

    virtual bool applyDiff(const DefinitionDiff *diff, bool backward = false) override;
    virtual void generateInitDiffs(vector<const DefinitionDiff *> *diffs) const override;

    /**
     * Set the maximal layer count allowed.
     */
    void setMaxLayerCount(int max_layer_count);

    /**
     * Get the current layer count.
     */
    int getLayerCount() const;

    /**
     * Retrieve a layer by its position.
     */
    DefinitionNode *getLayer(int position) const;

    /**
     * Retrieve a layer by its name.
     */
    DefinitionNode *getLayer(const string &name) const;

    /**
     * Add a new empty layer.
     */
    void addLayer(const string &name);

    /**
     * Add a new layer, copying another node into it.
     */
    void addLayer(const DefinitionNode &tocopy);

    /**
     * Remove a layer by its position.
     */
    void removeLayer(int position);

    /**
     * Clear this node of all layers.
     */
    void clear();

  public:
    LayerConstructor layer_constructor;
    int max_layer_count;
    vector<DefinitionNode *> layers;
    DefinitionNode *null_layer;
};
}
}
