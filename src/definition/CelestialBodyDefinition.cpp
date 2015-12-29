#include "CelestialBodyDefinition.h"

#include "Vector3.h"
#include "FloatNode.h"

CelestialBodyDefinition::CelestialBodyDefinition(DefinitionNode *parent, const string &name)
    : DefinitionNode(parent, name) {
    phi = new FloatNode(this, "phi");
    theta = new FloatNode(this, "theta");
    radius = new FloatNode(this, "radius");
}

Vector3 CelestialBodyDefinition::getDirection() const {
    VectorSpherical spc = {1.0, theta->getValue(), -phi->getValue()};
    return Vector3(spc);
}

// VectorSpherical moon_location_s = {MOON_DISTANCE_SCALED, atmosphere->moon_theta, -atmosphere->moon_phi};
