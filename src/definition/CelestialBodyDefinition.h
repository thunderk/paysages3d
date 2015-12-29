#ifndef CELESTIALBODYDEFINITION_H
#define CELESTIALBODYDEFINITION_H

#include "definition_global.h"

#include "DefinitionNode.h"

namespace paysages {
namespace definition {

class DEFINITIONSHARED_EXPORT CelestialBodyDefinition : public DefinitionNode {
  public:
    CelestialBodyDefinition(DefinitionNode *parent, const string &name);

    inline FloatNode *propPhi() const {
        return phi;
    }
    inline FloatNode *propTheta() const {
        return theta;
    }
    inline FloatNode *propRadius() const {
        return radius;
    }

    /**
     * Get the normalized direction toward the celestial body (from the center of the earth).
     */
    Vector3 getDirection() const;

  private:
    FloatNode *phi;
    FloatNode *theta;
    FloatNode *radius;
};
}
}

#endif // CELESTIALBODYDEFINITION_H
