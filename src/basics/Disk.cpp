#include "Disk.h"

#include "PackStream.h"

Disk::Disk(const Vector3 &point, const Vector3 &normal, double radius):
    InfinitePlane(point, normal), radius(radius)
{
    radius2 = radius * radius;
}

int Disk::checkRayIntersection(const InfiniteRay &ray, Vector3 *intersection) const
{
    int result = InfinitePlane::checkRayIntersection(ray, intersection);

    if (result == 1)
    {
        Vector3 v = intersection->sub(getPoint());
        double d2 = v.dotProduct(v);
        return (d2 <= radius2) ? 1 : 0;
    }
    else
    {
        return result;
    }
}

void Disk::save(PackStream *stream) const
{
    InfinitePlane::save(stream);
    stream->write(&radius);
    stream->write(&radius2);
}

void Disk::load(PackStream *stream)
{
    InfinitePlane::load(stream);
    stream->read(&radius);
    stream->read(&radius2);
}
