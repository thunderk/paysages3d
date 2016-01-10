#include "GodRaysDefinition.h"

#include "FloatNode.h"

GodRaysDefinition::GodRaysDefinition(DefinitionNode *parent) : DefinitionNode(parent, "godrays", "godrays") {
    penetration = new FloatNode(this, "penetration", 0.01);
    resistance = new FloatNode(this, "resistance", 0.3);
    boost = new FloatNode(this, "boost", 3.0);
}
