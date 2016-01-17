#include "NightSky.h"

#include <algorithm>
#include "Color.h"
#include "Vector3.h"
#include "Geometry.h"
#include "SoftwareRenderer.h"
#include "Scenery.h"
#include "AtmosphereDefinition.h"
#include "AtmosphereRenderer.h"
#include "SurfaceMaterial.h"
#include "LightComponent.h"
#include "LightStatus.h"
#include "CelestialBodyDefinition.h"
#include "FloatNode.h"

NightSky::NightSky(SoftwareRenderer *renderer) : renderer(renderer) {
}

NightSky::~NightSky() {
}

void NightSky::update() {
}

const Color NightSky::getColor(double altitude, const Vector3 &direction) {
    AtmosphereDefinition *atmosphere = renderer->getScenery()->getAtmosphere();
    Color result(0.01, 0.012, 0.03);

    Vector3 location(0.0, altitude, 0.0);

    // Get stars
    Vector3 sun_direction = renderer->getAtmosphereRenderer()->getSunDirection();
    if (sun_direction.y < 0.1) {
        double factor = (sun_direction.y < 0.0) ? 1.0 : 1.0 - (sun_direction.y * 10.0);
        for (const auto &star : atmosphere->stars) {
            if (star.location.dotProduct(direction) >= 0) {
                double radius = star.radius;
                Vector3 hit1, hit2;
                int hits = Geometry::rayIntersectSphere(location, direction, star.location, radius, &hit1, &hit2);
                if (hits > 1) {
                    double dist =
                        hit2.sub(hit1).getNorm() / radius; // distance between intersection points (relative to radius)

                    Color color = star.col;
                    if (dist <= 0.5) {
                        color.a *= (1.0 - dist / 0.5) * factor;
                    }
                    result.mask(color);
                }
            }
        }
    }

    return result;
}

bool NightSky::getLightsAt(vector<LightComponent> &result, const Vector3 &location) const {
    LightComponent sky;

    sky.color = Color(0.01, 0.012, 0.03).scaled(1.0 - min(location.y / Scenery::ATMOSPHERE_WIDTH_SCALED, 1.0));
    sky.direction = VECTOR_DOWN;
    sky.reflection = 0.0;
    sky.altered = 0;

    result.push_back(sky);

    return true;
}
