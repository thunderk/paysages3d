#include "IntDiff.h"

IntDiff::IntDiff(const DefinitionNode *node, int oldvalue, int newvalue)
    : DefinitionDiff(node), oldvalue(oldvalue), newvalue(newvalue) {
}
