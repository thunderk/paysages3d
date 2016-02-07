#include "MoonRenderer.h"

#include <cmath>
#include "CelestialBodyDefinition.h"
#include "Color.h"
#include "LightingManager.h"
#include "Geometry.h"
#include "SurfaceMaterial.h"
#include "FloatNode.h"
#include "FractalNoise.h"
#include "NoiseNode.h"

class MoonRenderer::pimpl {
  public:
    pimpl() : definition(NULL, "moon") {
    }
    CelestialBodyDefinition definition;
};

MoonRenderer::MoonRenderer(CelestialBodyDefinition *moon_node) : DefinitionWatcher("MoonRenderer"), impl(new pimpl()) {
    startWatchingPath(moon_node->getRoot(), moon_node->getPath());
}

MoonRenderer::~MoonRenderer() {
}

void MoonRenderer::nodeChanged(const DefinitionNode *, const DefinitionDiff *, const DefinitionNode *parent) {
    if (auto moon_node = dynamic_cast<const CelestialBodyDefinition *>(parent)) {
        moon_node->copy(&impl->definition);
    }
}

bool MoonRenderer::getLightsAt(vector<LightComponent> &result, const Vector3 &location) const {
    LightComponent light;

    // TODO Don't add if its contribution is negligible or below horizon
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

            auto p1 = nearest.sub(moon_location).normalize();
            auto coords = p1.toSpherical();

            auto noise = impl->definition.propNoise()->getGenerator();

            double precision = 0.00001;
            coords.phi += precision;
            auto p2 = Vector3(coords);
            p2 = p2.scale(1.0 - precision * fabs(noise->get3d(0.01, p2.x, p2.y, p2.z)));

            coords.phi -= precision;
            coords.theta += precision;
            auto p3 = Vector3(coords);
            p3 = p3.scale(1.0 - precision * fabs(noise->get3d(0.01, p3.x, p3.y, p3.z)));

            auto normal = p1.getNormal3(p2, p3);

            double gradient = 2.0 + 10.0 * noise->get3d(0.01, 0.3 * p1.x + 12.0, 0.3 * p1.y - 4.8, -0.3 * p1.z + 7.4);
            SurfaceMaterial material(Color(gradient, gradient, gradient));
            material.hardness = 0.3;

            auto moon_color = lighting->apply(eye, nearest, normal, material);
            if (dist <= 0.05) {
                moon_color.a *= 1.0 - dist / 0.05;
            }
            return moon_color;
        }
    }
    return COLOR_TRANSPARENT;
}
