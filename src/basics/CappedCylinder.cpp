#include "CappedCylinder.h"

#include "Vector3.h"
#include "PackStream.h"

CappedCylinder::CappedCylinder(const Vector3 &base, const Vector3 &direction, double radius, double length)
    : InfiniteCylinder(InfiniteRay(base, direction), radius), length(length) {
}

int CappedCylinder::checkRayIntersection(const InfiniteRay &ray, Vector3 *first_intersection,
                                         Vector3 *second_intersection) const {
    // TODO Apply the caps
    int count = InfiniteCylinder::checkRayIntersection(ray, first_intersection, second_intersection);

    if (count == 0) {
        return 0;
    } else if (count == 2) {
        if (checkPointProjection(first_intersection)) {
            if (checkPointProjection(second_intersection)) {
                return 2;
            } else {
                return 1;
            }
        } else {
            if (checkPointProjection(second_intersection)) {
                *first_intersection = *second_intersection;
                return 1;
            } else {
                return 0;
            }
        }
    } else // count == 1
    {
        if (checkPointProjection(first_intersection)) {
            return 1;
        } else {
            return 0;
        }
    }
}

bool CappedCylinder::checkPointProjection(Vector3 *point) const {
    double proj_length = axis.getDirection().dotProduct(point->sub(axis.getOrigin()));
    return 0.0 <= proj_length && proj_length <= length;
}

void CappedCylinder::save(PackStream *stream) const {
    InfiniteCylinder::save(stream);
    stream->write(&length);
}

void CappedCylinder::load(PackStream *stream) {
    InfiniteCylinder::load(stream);
    stream->read(&length);
}
