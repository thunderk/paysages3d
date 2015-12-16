#include "ColorDiff.h"

ColorDiff::ColorDiff(const DefinitionNode *node, const Color &oldvalue, const Color &newvalue)
    : DefinitionDiff(node), oldvalue(oldvalue), newvalue(newvalue) {
}

ColorDiff::ColorDiff(const ColorDiff *other, const Color &oldvalue, const Color &newvalue)
    : DefinitionDiff(other), oldvalue(oldvalue), newvalue(newvalue) {
}

DefinitionDiff *ColorDiff::newReversed() const {
    return new ColorDiff(this, newvalue, oldvalue);
}
