#ifndef INFINITERAY_H
#define INFINITERAY_H

#include "basics_global.h"

#include "Vector3.h"

namespace paysages {
namespace basics {

/**
 * Infinite ray (line).
 */
class BASICSSHARED_EXPORT InfiniteRay
{
public:
    InfiniteRay();
    InfiniteRay(const Vector3 &origin, const Vector3 &direction);

    static InfiniteRay fromPoints(const Vector3 &point1, const Vector3 &point2);

    inline const Vector3 &getOrigin() const {return origin;}
    inline const Vector3 &getDirection() const {return direction;}

    inline double getCursor(const Vector3 &point) const {return point.sub(origin).dotProduct(direction);}
    inline Vector3 getPointAtCursor(double distance) const {return origin.add(direction.scale(distance));}

    void save(PackStream *stream) const;
    void load(PackStream *stream);

private:
    Vector3 origin;
    Vector3 direction;
};

}
}

#endif // INFINITERAY_H
