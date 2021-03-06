#include "DefinitionDiff.h"

#include "DefinitionNode.h"

DefinitionDiff::DefinitionDiff(const DefinitionNode *node) : type_name(node->getTypeName()), path(node->getPath()) {
}

DefinitionDiff::DefinitionDiff(const DefinitionDiff *other) : type_name(other->type_name), path(other->path) {
}

DefinitionDiff::~DefinitionDiff() {
}
