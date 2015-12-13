#ifndef FLOATDIFF_H
#define FLOATDIFF_H

#include "definition_global.h"

#include "DefinitionDiff.h"

namespace paysages {
namespace definition {

/**
 * Diff for a FloatNode.
 */
class DEFINITIONSHARED_EXPORT FloatDiff : public DefinitionDiff {
  public:
    FloatDiff(const DefinitionNode *node, double oldvalue, double newvalue);
    FloatDiff(const FloatDiff *other, double oldvalue, double newvalue);

    inline double getOldValue() const {
        return oldvalue;
    }
    inline double getNewValue() const {
        return newvalue;
    }

    virtual DefinitionDiff *newReversed() const override;

  private:
    double oldvalue;
    double newvalue;
};
}
}

#endif // FLOATDIFF_H
