#include "NightSky.h"

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

#define WORLD_SCALING 0.05
#define MOON_DISTANCE 384403.0
#define MOON_DISTANCE_SCALED (MOON_DISTANCE / WORLD_SCALING)
#define MOON_RADIUS 1737.4
#define MOON_RADIUS_SCALED (MOON_RADIUS / WORLD_SCALING)

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

    // Get moon
    VectorSpherical moon_location_s = {MOON_DISTANCE_SCALED, atmosphere->moon_theta, -atmosphere->moon_phi};
    Vector3 moon_position(moon_location_s);
    Vector3 moon_direction = moon_position.normalize();
    if (moon_direction.dotProduct(direction) >= 0) {
        double moon_radius = MOON_RADIUS_SCALED * 5.0 * atmosphere->moon_radius;
        Vector3 hit1, hit2;
        int hits = Geometry::rayIntersectSphere(location, direction, moon_position, moon_radius, &hit1, &hit2);
        if (hits > 1) {
            double dist =
                hit2.sub(hit1).getNorm() / moon_radius; // distance between intersection points (relative to radius)

            Vector3 nearest = (hit1.sub(location).getNorm() > hit2.sub(location).getNorm()) ? hit2 : hit1;
            SurfaceMaterial moon_material(Color(3.0, 3.0, 3.0));
            moon_material.validate();

            Color moon_color =
                renderer->applyLightingToSurface(nearest, nearest.sub(moon_position).normalize(), moon_material);
            if (dist <= 0.05) {
                moon_color.a *= 1.0 - dist / 0.05;
            }
            result.mask(moon_color);
        }
    }

    return result;
}

bool NightSky::getLightsAt(std::vector<LightComponent> &result, const Vector3 &) const {
    LightComponent moon, sky;

    AtmosphereDefinition *atmosphere = renderer->getScenery()->getAtmosphere();
    VectorSpherical moon_location_s = {MOON_DISTANCE_SCALED, atmosphere->moon_theta, -atmosphere->moon_phi};

    moon.color = Color(0.03, 0.03, 0.03); // TODO take moon phase into account
    moon.direction = Vector3(moon_location_s).normalize().scale(-1.0);
    moon.reflection = 0.2;
    moon.altered = 1;

    result.push_back(moon);

    sky.color = Color(0.01, 0.012, 0.03);
    sky.direction = VECTOR_DOWN;
    sky.reflection = 0.0;
    sky.altered = 0;

    result.push_back(sky);

    return true;
}
