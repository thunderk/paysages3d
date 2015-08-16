#include "DefinitionDiff.h"

#include "DefinitionNode.h"

DefinitionDiff::DefinitionDiff(const DefinitionNode *node):
    type_name(node->getTypeName()), path(node->getPath())
{
}
