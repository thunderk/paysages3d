#pragma once

#include "definition_global.h"

#include "DefinitionDiff.h"

namespace paysages {
namespace definition {

/**
 * Diff for an IntNode.
 */
class DEFINITIONSHARED_EXPORT IntDiff : public DefinitionDiff {
  public:
    IntDiff(const DefinitionNode *node, int oldvalue, int newvalue);
    IntDiff(const IntDiff *other, int oldvalue, int newvalue);

    inline int getOldValue() const {
        return oldvalue;
    }
    inline int getNewValue() const {
        return newvalue;
    }

    virtual DefinitionDiff *newReversed() const override;

  private:
    int oldvalue;
    int newvalue;
};
}
}
