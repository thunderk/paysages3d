#ifndef CELESTIALBODYDEFINITION_H
#define CELESTIALBODYDEFINITION_H

#include "definition_global.h"

#include "DefinitionNode.h"

namespace paysages {
namespace definition {

class DEFINITIONSHARED_EXPORT CelestialBodyDefinition : public DefinitionNode {
  public:
    CelestialBodyDefinition(DefinitionNode *parent, const string &name);

    inline FloatNode *propDistance() const {
        return distance;
    }
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
     * Get the normalized direction of the celestial body, from the center of the earth.
     */
    Vector3 getGlobalDirection() const;

    /**
     * Get the location of the celestial body.
     *
     * If "over_water" is true, the location is given in standard coordinates with y=0.0 as water,
     * otherwise in regard to the earth center.
     */
    Vector3 getLocation(bool over_water = true) const;

    /**
     * Get the angular radius, when viewed from earth.
     */
    double getAngularRadius() const;

  private:
    FloatNode *distance;
    FloatNode *phi;
    FloatNode *theta;
    FloatNode *radius;
};
}
}

#endif // CELESTIALBODYDEFINITION_H
