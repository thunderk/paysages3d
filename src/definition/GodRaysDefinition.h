#ifndef GODRAYSDEFINITION_H
#define GODRAYSDEFINITION_H

#include "definition_global.h"

#include "DefinitionNode.h"

namespace paysages {
namespace definition {

class DEFINITIONSHARED_EXPORT GodRaysDefinition : public DefinitionNode {
  public:
    GodRaysDefinition(DefinitionNode *parent);

    inline FloatNode *propPenetration() const {
        return penetration;
    }
    inline FloatNode *propResistance() const {
        return resistance;
    }
    inline FloatNode *propBoost() const {
        return boost;
    }

  private:
    FloatNode *penetration;
    FloatNode *resistance;
    FloatNode *boost;
};
}
}

#endif // GODRAYSDEFINITION_H
