#pragma once

#include "definition_global.h"

#include "DefinitionDiff.h"
#include "Color.h"

namespace paysages {
namespace definition {

/**
 * Diff for a ColorNode.
 */
class DEFINITIONSHARED_EXPORT ColorDiff : public DefinitionDiff {
  public:
    ColorDiff(const DefinitionNode *node, const Color &oldvalue, const Color &newvalue);
    ColorDiff(const ColorDiff *other, const Color &oldvalue, const Color &newvalue);

    inline const Color &getOldValue() const {
        return oldvalue;
    }
    inline const Color &getNewValue() const {
        return newvalue;
    }

    virtual DefinitionDiff *newReversed() const override;

  private:
    Color oldvalue;
    Color newvalue;
};
}
}
