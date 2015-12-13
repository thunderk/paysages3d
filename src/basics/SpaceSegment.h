#ifndef SPACESEGMENT_H
#define SPACESEGMENT_H

#include "basics_global.h"

#include "Vector3.h"

namespace paysages {
namespace basics {

/**
 * A delimited segment in 3D space (mainly useful for rays).
 */
class BASICSSHARED_EXPORT SpaceSegment {
  public:
    SpaceSegment(const Vector3 &start, const Vector3 &end);
    SpaceSegment() : SpaceSegment(Vector3(), Vector3()) {
    }

    inline const Vector3 &getStart() const {
        return start;
    }
    inline const Vector3 &getEnd() const {
        return end;
    }
    inline Vector3 getDirection() const {
        return end.sub(start);
    }
    inline double getLength() const {
        return end.sub(start).getNorm();
    }

    inline double getXDiff() const {
        return end.x - start.x;
    }
    inline double getYDiff() const {
        return end.y - start.y;
    }
    inline double getZDiff() const {
        return end.z - start.z;
    }

    /**
     * Keep only the intersection with a slice orthogonal to the Y axis.
     *
     * Return true if a segment remains.
     */
    bool intersectYInterval(double ymin, double ymax);

    /**
     * Return true if the segment intersects a bounding box, represented by another segment (crossing diagonal).
     */
    bool intersectBoundingBox(const SpaceSegment &bbox) const;

    /**
     * Return a version of this segment, projected on a X plane.
     */
    SpaceSegment projectedOnXPlane(double x = 0.0) const;
    /**
     * Return a version of this segment, projected on a Y plane.
     */
    SpaceSegment projectedOnYPlane(double y = 0.0) const;
    /**
     * Return a version of this segment, projected on a Z plane.
     */
    SpaceSegment projectedOnZPlane(double z = 0.0) const;

    /**
     * Return a scaled version of this segment.
     *
     * Pay attention, scaling is done from the coordinates origin (0,0,0), not the segment center.
     */
    SpaceSegment scaled(double factor) const;

    /**
     * Iterate inside a virtual grid.
     *
     * The space is considered as cut into 1.0-sized cubic cells,
     * and the delegate will be called with coordinates of the cells
     * traversed by this segment.
     *
     * Return false if the iteration was aborted by the delegate, true otherwise.
     */
    bool iterateOnGrid(SpaceGridIterator &delegate);

  private:
    Vector3 start;
    Vector3 end;
};
}
}

#endif // SPACESEGMENT_H
