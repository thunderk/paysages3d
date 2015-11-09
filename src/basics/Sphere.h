#ifndef SPHERE_H
#define SPHERE_H

#include "basics_global.h"

#include "Vector3.h"

namespace paysages {
namespace basics {

/**
 * Geometric sphere.
 */
class BASICSSHARED_EXPORT Sphere {
  public:
    Sphere() = default;
    Sphere(const Vector3 &center, double radius);

    inline const Vector3 &getCenter() const {
        return center;
    }
    inline const double &getRadius() const {
        return radius;
    }

    /**
     * Check the intersection between the sphere and an infinite ray.
     *
     * Returns the number of intersections (0, 1 or 2) and fill the intersection points.
     */
    int checkRayIntersection(const InfiniteRay &ray, Vector3 *first_intersection, Vector3 *second_intersection) const;

    void save(PackStream *stream) const;
    void load(PackStream *stream);

  private:
    Vector3 center;
    double radius;
    double radius2;
};
}
}

#endif // SPHERE_H
