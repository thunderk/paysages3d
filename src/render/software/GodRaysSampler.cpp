#include "GodRaysSampler.h"

#include <cmath>
#include <algorithm>
#include "GodRaysDefinition.h"
#include "AtmosphereDefinition.h"
#include "SoftwareRenderer.h"
#include "Vector3.h"
#include "SpaceSegment.h"
#include "GodRaysResult.h"
#include "LightingManager.h"
#include "FloatNode.h"
#include "LightStatus.h"
#include "Scenery.h"
#include "TerrainDefinition.h"
#include "CloudsRenderer.h"
#include "Interpolation.h"

GodRaysSampler::GodRaysSampler() {
    enabled = true;
    bounds = new SpaceSegment();
    definition = new GodRaysDefinition(NULL);
    camera_location = new Vector3(0, 0, 0);
    lighting = NULL;
    low_altitude = -1.0;
    high_altitude = 1.0;
    sampling_step = 1.0;
    max_length = 1.0;
    data = new double[1];
    setQuality(0.5);
}

GodRaysSampler::~GodRaysSampler() {
    delete definition;
    delete bounds;
    delete camera_location;
    delete[] data;
}

void GodRaysSampler::prepare(SoftwareRenderer *renderer) {
    setCameraLocation(renderer->getCameraLocation());
    setLighting(renderer->getLightingManager());
    setAltitudes(renderer->getScenery()->getTerrain()->getHeightInfo().min_height,
                 renderer->getCloudsRenderer()->getHighestAltitude());
    renderer->getScenery()->getAtmosphere()->childGodRays()->copy(definition);
    reset();
}

void GodRaysSampler::reset() {
    *bounds = SpaceSegment(Vector3(camera_location->x - max_length, low_altitude, camera_location->z - max_length),
                           Vector3(camera_location->x + max_length, high_altitude, camera_location->z + max_length));
    samples_x = round_to_int(bounds->getXDiff() / sampling_step) + 1;
    samples_y = round_to_int(bounds->getYDiff() / sampling_step) + 1;
    samples_z = round_to_int(bounds->getZDiff() / sampling_step) + 1;

    auto n = to_size(samples_x * samples_y * samples_z);
    delete[] data;
    data = new double[n];
    fill_n(data, n, -1.0);
}

void GodRaysSampler::setEnabled(bool enabled) {
    this->enabled = enabled;
}

void GodRaysSampler::setLighting(LightingManager *manager) {
    this->lighting = manager;
}

void GodRaysSampler::setQuality(double factor) {
    setQuality(5.0 / (factor * 8.0 + 1.0), 100.0, 5.0 / (factor * 80.0 + 1.0));
    reset();
}

void GodRaysSampler::setQuality(double sampling_step, double max_length, double walk_step) {
    this->sampling_step = sampling_step;
    this->max_length = max_length;
    this->walk_step = walk_step;
}

void GodRaysSampler::setCameraLocation(const Vector3 &location) {
    *camera_location = location;
}

void GodRaysSampler::setAltitudes(double low, double high) {
    low_altitude = low;
    high_altitude = high;
}

void GodRaysSampler::getSamples(int *x, int *y, int *z) const {
    *x = samples_x;
    *y = samples_y;
    *z = samples_z;
}

Color GodRaysSampler::getRawLight(const Vector3 &location, bool filtered) const {
    if (lighting) {
        LightStatus status(lighting, location, *camera_location, filtered);
        status.setSafetyOffset(0.0); // Not a hard surface, safety offset not needed
        lighting->fillStatus(status, location);
        return status.getSum();
    } else {
        return COLOR_TRANSPARENT;
    }
}

double GodRaysSampler::getCachedLight(const Vector3 &location) {
    double x = location.x - bounds->getStart().x;
    double y = location.y - bounds->getStart().y;
    double z = location.z - bounds->getStart().z;

    int ix = floor_to_int(x / sampling_step);
    int iy = floor_to_int(y / sampling_step);
    int iz = floor_to_int(z / sampling_step);

    // Check cache limits
    if (ix < 0 || ix >= samples_x - 1 || iy < 0 || iy >= samples_y - 1 || iz < 0 || iz >= samples_z - 1) {
        return 1.0;
    }

    // Hit cache
    double p[8] = {getCache(ix, iy, iz), getCache(ix + 1, iy, iz), getCache(ix + 1, iy + 1, iz),
                   getCache(ix, iy + 1, iz), getCache(ix, iy, iz + 1), getCache(ix + 1, iy, iz + 1),
                   getCache(ix + 1, iy + 1, iz + 1), getCache(ix, iy + 1, iz + 1)};
    return Interpolation::trilinear(p, (x - sampling_step * double(ix)) / sampling_step,
                                    (y - sampling_step * double(iy)) / sampling_step,
                                    (z - sampling_step * double(iz)) / sampling_step);
}

GodRaysResult GodRaysSampler::getResult(const SpaceSegment &segment) {
    Vector3 step = segment.getEnd().sub(segment.getStart());
    double max_length = step.getNorm();
    step = step.normalize().scale(walk_step);
    Vector3 walker = segment.getStart();
    double travelled = 0.0;
    double inside = 0.0;

    if (max_length > this->max_length) {
        max_length = this->max_length;
    }

    while (travelled < max_length) {
        double light = getCachedLight(walker);

        inside += light * walk_step;

        walker = walker.add(step);
        travelled += walk_step;
    }

    return GodRaysResult(inside, travelled);
}

Color GodRaysSampler::apply(const Color &raw, const Color &atmosphered, const Vector3 &location) {
    if (enabled) {
        GodRaysResult result = getResult(SpaceSegment(*camera_location, location));

        GodRaysResult::GodRaysParams params;
        params.penetration = definition->propPenetration()->getValue();
        params.resistance = definition->propResistance()->getValue();
        params.boost = definition->propBoost()->getValue();

        return result.apply(raw, atmosphered, params);
    } else {
        return atmosphered;
    }
}

inline double GodRaysSampler::getCache(int x, int y, int z) {
    double *cache = data + z * samples_x * samples_y + y * samples_x + x;
    if (*cache < 0.0) {
        Vector3 location = Vector3(bounds->getStart().x + sampling_step * to_double(x),
                                   bounds->getStart().y + sampling_step * to_double(y),
                                   bounds->getStart().z + sampling_step * to_double(z));
        double unfiltered_power = getRawLight(location, false).getPower();
        if (unfiltered_power == 0.0) {
            *cache = 1.0;
        } else {
            *cache = getRawLight(location, true).getPower() / unfiltered_power;
        }
    }
    return *cache;
}
