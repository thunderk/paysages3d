#include "InfiniteRay.h"

InfiniteRay::InfiniteRay()
{
}

InfiniteRay::InfiniteRay(const Vector3 &origin, const Vector3 &direction):
    origin(origin), direction(direction.normalize())
{
}

InfiniteRay InfiniteRay::fromPoints(const Vector3 &point1, const Vector3 &point2)
{
    return InfiniteRay(point1, point2.sub(point1).normalize());
}

void InfiniteRay::save(PackStream *stream) const
{
    origin.save(stream);
    direction.save(stream);
}

void InfiniteRay::load(PackStream *stream)
{
    origin.load(stream);
    direction.load(stream);
}
