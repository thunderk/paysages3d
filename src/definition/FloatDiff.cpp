#include "FloatDiff.h"

FloatDiff::FloatDiff(const DefinitionNode *node, double oldvalue, double newvalue)
    : DefinitionDiff(node), oldvalue(oldvalue), newvalue(newvalue) {
}

FloatDiff::FloatDiff(const FloatDiff *other, double oldvalue, double newvalue)
    : DefinitionDiff(other), oldvalue(oldvalue), newvalue(newvalue) {
}

DefinitionDiff *FloatDiff::newReversed() const {
    return new FloatDiff(this, newvalue, oldvalue);
}
