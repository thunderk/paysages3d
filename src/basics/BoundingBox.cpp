#include "BoundingBox.h"

#include "Vector3.h"

BoundingBox::BoundingBox()
{
    reset();
}

void BoundingBox::reset()
{
    empty = 1;
    xmin = 10000000000.0;
    xmax = -10000000000.0;
    ymin = 10000000000.0;
    ymax = -10000000000.0;
    zmin = 10000000000.0;
    zmax = -10000000000.0;
}

void BoundingBox::pushPoint(const Vector3 &point)
{
    empty = 0;
    if (point.x < xmin)
    {
        xmin = point.x;
    }
    if (point.x > xmax)
    {
        xmax = point.x;
    }
    if (point.y < ymin)
    {
        ymin = point.y;
    }
    if (point.y > ymax)
    {
        ymax = point.y;
    }
    if (point.z < zmin)
    {
        zmin = point.z;
    }
    if (point.z > zmax)
    {
        zmax = point.z;
    }
}
