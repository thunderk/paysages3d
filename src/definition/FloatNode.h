#ifndef FLOATNODE_H
#define FLOATNODE_H

#include "definition_global.h"

#include "DefinitionNode.h"

namespace paysages {
namespace definition {

/**
 * Node with a single floating point numeric value, for the definition tree.
 */
class DEFINITIONSHARED_EXPORT FloatNode : public DefinitionNode {
  public:
    FloatNode(DefinitionNode *parent, const string &name, double value = 0.0);

    inline double getValue() const {
        return value;
    }

    virtual string toString(int indent) const override;
    virtual void save(PackStream *stream) const override;
    virtual void load(PackStream *stream) override;
    virtual void copy(DefinitionNode *destination) const override;

    /**
     * Change the float value stored.
     *
     * The DiffManager is used as intermediary, so that the change may not happen immediately.
     */
    void setValue(double new_value);
    const FloatDiff *produceDiff(double new_value) const;
    virtual void generateInitDiffs(vector<const DefinitionDiff *> *diffs) const override;
    virtual bool applyDiff(const DefinitionDiff *diff, bool backward = false) override;

    void addValue(double added);

  private:
    double value;
};
}
}

#endif // FLOATNODE_H
