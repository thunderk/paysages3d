#include "MoonRenderer.h"

#include "CelestialBodyDefinition.h"
#include "Color.h"
#include "LightingManager.h"
#include "Geometry.h"
#include "SurfaceMaterial.h"
#include "FloatNode.h"

class MoonRenderer::pimpl {
  public:
    pimpl() : definition(NULL, "moon"), material(Color(3.0, 3.0, 3.0)) {
        // TODO Put material in scenery
    }
    CelestialBodyDefinition definition;
    SurfaceMaterial material;
};

MoonRenderer::MoonRenderer(CelestialBodyDefinition *moon_node) : impl(new pimpl()) {
    startWatching(moon_node->getRoot(), moon_node->getPath());

    // FIXME should not be needed because the above watcher should watch the whole node
    //   and call nodeChanged
    moon_node->copy(&impl->definition);
}

MoonRenderer::~MoonRenderer() {
}

void MoonRenderer::nodeChanged(const DefinitionNode *node, const DefinitionDiff *) {
    if (auto moon_node = static_cast<const CelestialBodyDefinition *>(node)) {
        moon_node->copy(&impl->definition);
    }
}

bool MoonRenderer::getLightsAt(vector<LightComponent> &result, const Vector3 &location) const {
    LightComponent light;

    // TODO Don't add if its contribution is negligible
    // TODO Take moon phase into account

    light.color = Color(0.03, 0.03, 0.03);
    light.direction = location.sub(impl->definition.getLocation()).normalize();
    light.reflection = 0.2;
    light.altered = true;

    result.push_back(light);
    return true;
}

Color MoonRenderer::getColor(const Vector3 &eye, const Vector3 &direction, LightingManager *lighting) const {
    auto moon_location = impl->definition.getLocation();
    auto moon_direction = moon_location.sub(eye).normalize();
    if (moon_direction.dotProduct(direction) >= 0) {
        // TODO Alter radius near horizon (lens effect)
        double moon_radius = impl->definition.propRadius()->getValue();
        Vector3 hit1, hit2;
        int hits = Geometry::rayIntersectSphere(eye, direction, moon_location, moon_radius, &hit1, &hit2);
        if (hits > 1) {
            // distance between intersection points (relative to radius)
            double dist = hit2.sub(hit1).getNorm() / moon_radius;
            auto nearest = (hit1.sub(eye).getNorm() > hit2.sub(eye).getNorm()) ? hit2 : hit1;

            auto moon_color = lighting->apply(eye, nearest, nearest.sub(moon_location).normalize(), impl->material);
            if (dist <= 0.05) {
                moon_color.a *= 1.0 - dist / 0.05;
            }
            return moon_color;
        }
    }
    return COLOR_TRANSPARENT;
}
