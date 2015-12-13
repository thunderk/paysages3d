#include "IntDiff.h"

IntDiff::IntDiff(const DefinitionNode *node, int oldvalue, int newvalue)
    : DefinitionDiff(node), oldvalue(oldvalue), newvalue(newvalue) {
}

IntDiff::IntDiff(const IntDiff *other, int oldvalue, int newvalue)
    : DefinitionDiff(other), oldvalue(oldvalue), newvalue(newvalue) {
}

DefinitionDiff *IntDiff::newReversed() const {
    return new IntDiff(this, newvalue, oldvalue);
}
