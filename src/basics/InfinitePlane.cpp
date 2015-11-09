#include "InfinitePlane.h"

#include "PackStream.h"
#include "InfiniteRay.h"

InfinitePlane::InfinitePlane() {
}

InfinitePlane::InfinitePlane(const Vector3 &point, const Vector3 &normal) : point(point), normal(normal) {
}

int InfinitePlane::checkRayIntersection(const InfiniteRay &ray, Vector3 *intersection) const {
    Vector3 p1 = ray.getDirection();
    double d = normal.dotProduct(p1);

    if (fabs(d) < 1e-8) {
        if (normal.dotProduct(ray.getPointAtCursor(1.0).sub(point)) == 0) {
            return -1;
        } else {
            return 0;
        }
    }

    double u = point.sub(ray.getOrigin()).dotProduct(normal) / d;
    *intersection = ray.getPointAtCursor(u);
    return 1;
}

void InfinitePlane::save(PackStream *stream) const {
    point.save(stream);
    normal.save(stream);
}

void InfinitePlane::load(PackStream *stream) {
    point.load(stream);
    normal.load(stream);
}
