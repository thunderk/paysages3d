#include "CelestialBodyDefinition.h"

#include "FloatNode.h"
#include "NoiseNode.h"
#include "Scenery.h"
#include "Vector3.h"
#include <cmath>

CelestialBodyDefinition::CelestialBodyDefinition(DefinitionNode *parent, const string &name)
    : DefinitionNode(parent, name) {
    distance = new FloatNode(this, "distance");
    phi = new FloatNode(this, "phi");
    theta = new FloatNode(this, "theta");
    radius = new FloatNode(this, "radius");
    noise = new NoiseNode(this, "noise");
}

Vector3 CelestialBodyDefinition::getGlobalDirection() const {
    VectorSpherical spc = {1.0, theta->getValue(), -phi->getValue()};
    return Vector3(spc);
}

Vector3 CelestialBodyDefinition::getLocation(bool over_water) const {
    VectorSpherical spc = {distance->getValue(), theta->getValue(), -phi->getValue()};
    if (over_water) {
        return Vector3(spc).sub(VECTOR_DOWN.scale(Scenery::EARTH_RADIUS_SCALED));
    } else {
        return Vector3(spc);
    }
}

double CelestialBodyDefinition::getAngularRadius() const {
    return 2.0 * atan(radius->getValue() / distance->getValue());
}
