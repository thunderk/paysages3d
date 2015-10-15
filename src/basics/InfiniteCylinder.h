#ifndef INFINITECYLINDER_H
#define INFINITECYLINDER_H

#include "basics_global.h"

#include "InfiniteRay.h"

namespace paysages {
namespace basics {

/**
 * Geometric cylinder, with infinite length.
 */
class BASICSSHARED_EXPORT InfiniteCylinder
{
public:
    InfiniteCylinder();
    InfiniteCylinder(const InfiniteRay &axis, double radius);

    inline const InfiniteRay& getAxis() const {return axis;}
    inline double getRadius() const {return radius;}

    /**
     * Check the intersection between the cylinder and an infinite ray.
     *
     * Returns the number of intersections (0, 1 or 2) and fill the intersection points.
     */
    int checkRayIntersection(const InfiniteRay& ray, Vector3 *first_intersection, Vector3 *second_intersection) const;

    virtual void save(PackStream *stream) const;
    virtual void load(PackStream *stream);

protected:
    InfiniteRay axis;
    double radius;
};

}
}

#endif // INFINITECYLINDER_H
