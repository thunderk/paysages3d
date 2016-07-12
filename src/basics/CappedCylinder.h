#pragma once

#include "basics_global.h"

#include "InfiniteCylinder.h"
#include "Sphere.h"

namespace paysages {
namespace basics {

/**
 * Geometric cylinder, with capped ends (not infinite).
 */
class BASICSSHARED_EXPORT CappedCylinder : public InfiniteCylinder {
  public:
    CappedCylinder() = default;
    CappedCylinder(const Vector3 &base, const Vector3 &direction, double radius, double length);

    inline double getLength() const {
        return length;
    }

    /**
     * Check the intersection between the cylinder and an infinite ray.
     *
     * Returns the number of intersections (0, 1 or 2) and fill the intersection points.
     */
    int findRayIntersection(const InfiniteRay &ray, Vector3 *first_intersection, Vector3 *second_intersection) const;

    /**
     * Check if a point projects in the length of the finite cylinder.
     */
    bool checkPointProjection(Vector3 *point) const;

    virtual void save(PackStream *stream) const override;
    virtual void load(PackStream *stream) override;

  private:
    double length;
    Sphere container;
};
}
}
