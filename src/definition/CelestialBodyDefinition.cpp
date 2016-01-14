#include "CelestialBodyDefinition.h"

#include "Vector3.h"
#include "FloatNode.h"

static constexpr double WORLD_SCALING = 0.05;
static constexpr double EARTH_RADIUS = 6360.0;
static constexpr double EARTH_RADIUS_SCALED = EARTH_RADIUS / WORLD_SCALING;

CelestialBodyDefinition::CelestialBodyDefinition(DefinitionNode *parent, const string &name)
    : DefinitionNode(parent, name) {
    distance = new FloatNode(this, "distance");
    phi = new FloatNode(this, "phi");
    theta = new FloatNode(this, "theta");
    radius = new FloatNode(this, "radius");
}

Vector3 CelestialBodyDefinition::getLocation(bool over_water) const {
    VectorSpherical spc = {distance->getValue(), theta->getValue(), -phi->getValue()};
    if (over_water) {
        return Vector3(spc).sub(VECTOR_DOWN.scale(EARTH_RADIUS_SCALED));
    } else {
        return Vector3(spc);
    }
}
