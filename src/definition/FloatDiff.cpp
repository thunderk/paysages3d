#include "FloatDiff.h"

FloatDiff::FloatDiff(const DefinitionNode *node, double oldvalue, double newvalue)
    : DefinitionDiff(node), oldvalue(oldvalue), newvalue(newvalue) {
}
