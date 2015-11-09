#ifndef INFINITEPLANE_H
#define INFINITEPLANE_H

#include "basics_global.h"

#include "Vector3.h"

namespace paysages {
namespace basics {

/**
 * Infinite 3d geometric plane.
 */
class BASICSSHARED_EXPORT InfinitePlane {
  public:
    InfinitePlane();
    InfinitePlane(const Vector3 &point, const Vector3 &normal);

    /**
     * Check the intersection between the plane and an infinite ray.
     *
     * Returns the number of intersections (0, 1), and fill the intersection point.
     *
     * Returns -1 if the ray is on the plane, and make for an infinite number of intersection points.
     */
    int checkRayIntersection(const InfiniteRay &ray, Vector3 *intersection) const;

    inline const Vector3 &getPoint() const {
        return point;
    }
    inline const Vector3 &getNormal() const {
        return normal;
    }

    virtual void save(PackStream *stream) const;
    virtual void load(PackStream *stream);

  private:
    Vector3 point;
    Vector3 normal;
};
}
}

#endif // INFINITEPLANE_H
