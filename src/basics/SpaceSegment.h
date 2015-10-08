#ifndef SPACESEGMENT_H
#define SPACESEGMENT_H

#include "basics_global.h"

#include "Vector3.h"

namespace paysages {
namespace basics {

/*!
 * \brief A segment in 3D space (mainly useful for rays).
 */
class BASICSSHARED_EXPORT SpaceSegment
{
public:
    SpaceSegment(const Vector3& start, const Vector3& end);
    SpaceSegment(): SpaceSegment(Vector3(), Vector3()) {}

    inline Vector3 getStart() const {return start;}
    inline Vector3 getEnd() const {return end;}

    inline double getXDiff() const {return end.x - start.x;}
    inline double getYDiff() const {return end.y - start.y;}
    inline double getZDiff() const {return end.z - start.z;}

    /*!
     * \brief Keep only the intersection with a slice orthogonal to the Y axis.
     * \return true if a segment remains
     */
    bool intersectYInterval(double ymin, double ymax);

private:
    Vector3 start;
    Vector3 end;
};

}
}

#endif // SPACESEGMENT_H
