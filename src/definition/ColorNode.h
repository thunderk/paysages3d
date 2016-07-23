#pragma once

#include "definition_global.h"

#include "Color.h"
#include "DefinitionNode.h"

namespace paysages {
namespace definition {

/**
 * Node with a single color value, for the definition tree.
 */
class DEFINITIONSHARED_EXPORT ColorNode : public DefinitionNode {
  public:
    ColorNode(DefinitionNode *parent, const string &name, const Color &value = COLOR_TRANSPARENT);

    inline const Color &getValue() const {
        return value;
    }

    virtual string toString(int indent) const override;
    virtual void save(PackStream *stream) const override;
    virtual void load(PackStream *stream) override;
    virtual void copy(DefinitionNode *destination) const override;

    /**
     * Change the color value stored.
     *
     * The DiffManager is used as intermediary, so that the change may not happen immediately.
     */
    void setValue(const Color &value);
    const ColorDiff *produceDiff(const Color &value) const;
    virtual void generateInitDiffs(vector<const DefinitionDiff *> *diffs) const override;
    virtual bool applyDiff(const DefinitionDiff *diff, bool backward = false) override;

  private:
    Color value;
};
}
}
