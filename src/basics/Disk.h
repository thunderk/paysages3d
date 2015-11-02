#ifndef DISK_H
#define DISK_H

#include "basics_global.h"

#include "InfinitePlane.h"

namespace paysages {
namespace basics {

/**
 * Geometric plane disk.
 */
class BASICSSHARED_EXPORT Disk: public InfinitePlane
{
public:
    Disk() = default;
    Disk(const Vector3 &point, const Vector3 &normal, double radius);

    inline double getRadius() const {return radius;}

    /**
     * Check the intersection between the disk and an infinite ray.
     *
     * Returns the number of intersections (0, 1), and fill the intersection point.
     *
     * Returns -1 if the ray shares a segment with the disk.
     */
    int checkRayIntersection(const InfiniteRay& ray, Vector3 *intersection) const;

    void save(PackStream *stream) const override;
    void load(PackStream *stream) override;

private:
    double radius;
    double radius2;
};

}
}

#endif // DISK_H
