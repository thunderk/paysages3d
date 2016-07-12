#pragma once

#include "basics_global.h"

#include "InfiniteRay.h"

namespace paysages {
namespace basics {

/**
 * Geometric cylinder, with infinite length.
 */
class BASICSSHARED_EXPORT InfiniteCylinder {
  public:
    InfiniteCylinder() = default;
    InfiniteCylinder(const InfiniteRay &axis, double radius);
    virtual ~InfiniteCylinder();

    inline const InfiniteRay &getAxis() const {
        return axis;
    }
    inline double getRadius() const {
        return radius;
    }

    /**
     * Check the intersection between the cylinder and an infinite ray.
     *
     * Returns the number of intersections (0, 1 or 2) and fill the intersection points.
     */
    int findRayIntersection(const InfiniteRay &ray, Vector3 *first_intersection, Vector3 *second_intersection) const;

    virtual void save(PackStream *stream) const;
    virtual void load(PackStream *stream);

  private:
    void validate();

  protected:
    InfiniteRay axis;
    double radius;

  private:
    // Stored equation factors, to speed up ray intersection
    double R[3][3];
    double A[3][3];
    double C;
    double ox;
    double oy;
    double oz;
};
}
}
