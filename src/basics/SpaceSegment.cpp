#include "SpaceSegment.h"

SpaceSegment::SpaceSegment(const Vector3& start, const Vector3& end):
    start(start), end(end)
{
}

bool SpaceSegment::intersectYInterval(double ymin, double ymax)
{
    if (start.y > ymax)
    {
        if (end.y >= ymax)
        {
            return false;
        }
        else
        {
            Vector3 diff = end.sub(start);
            start = start.add(diff.scale((ymax - start.y) / diff.y));
            if (end.y < ymin)
            {
                end = end.add(diff.scale((ymin - end.y) / diff.y));
            }
        }
    }
    else if (start.y < ymin)
    {
        if (end.y <= ymin)
        {
            return false;
        }
        else
        {
            Vector3 diff = end.sub(start);
            start = start.add(diff.scale((ymin - start.y) / diff.y));
            if (end.y >= ymax)
            {
                end = end.add(diff.scale((ymax - end.y) / diff.y));
            }
        }
    }
    else /* start is inside layer */
    {
        Vector3 diff = end.sub(start);
        if (end.y > ymax)
        {
            end = start.add(diff.scale((ymax - start.y) / diff.y));
        }
        else if (end.y < ymin)
        {
            end = start.add(diff.scale((ymin - start.y) / diff.y));
        }
    }

    return true;
}
