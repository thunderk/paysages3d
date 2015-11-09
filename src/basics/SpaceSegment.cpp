#include "SpaceSegment.h"

#include <climits>
#include "SpaceGridIterator.h"

SpaceSegment::SpaceSegment(const Vector3 &start, const Vector3 &end) : start(start), end(end) {
}

bool SpaceSegment::intersectYInterval(double ymin, double ymax) {
    if (start.y > ymax) {
        if (end.y >= ymax) {
            return false;
        } else {
            Vector3 diff = end.sub(start);
            start = start.add(diff.scale((ymax - start.y) / diff.y));
            if (end.y < ymin) {
                end = end.add(diff.scale((ymin - end.y) / diff.y));
            }
        }
    } else if (start.y < ymin) {
        if (end.y <= ymin) {
            return false;
        } else {
            Vector3 diff = end.sub(start);
            start = start.add(diff.scale((ymin - start.y) / diff.y));
            if (end.y >= ymax) {
                end = end.add(diff.scale((ymax - end.y) / diff.y));
            }
        }
    } else /* start is inside layer */
    {
        Vector3 diff = end.sub(start);
        if (end.y > ymax) {
            end = start.add(diff.scale((ymax - start.y) / diff.y));
        } else if (end.y < ymin) {
            end = start.add(diff.scale((ymin - start.y) / diff.y));
        }
    }

    return true;
}

SpaceSegment SpaceSegment::projectedOnXPlane(double x) const {
    return SpaceSegment(Vector3(x, start.y, start.z), Vector3(x, end.y, end.z));
}

SpaceSegment SpaceSegment::projectedOnYPlane(double y) const {
    return SpaceSegment(Vector3(start.x, y, start.z), Vector3(end.x, y, end.z));
}

SpaceSegment SpaceSegment::projectedOnZPlane(double z) const {
    return SpaceSegment(Vector3(start.x, start.y, z), Vector3(end.x, end.y, z));
}

SpaceSegment SpaceSegment::scaled(double factor) const {
    return SpaceSegment(start.scale(factor), end.scale(factor));
}

bool SpaceSegment::iterateOnGrid(SpaceGridIterator &delegate) {
    Vector3 diff = end.sub(start);

    int stepX = diff.x < 0.0 ? -1 : 1;
    int stepY = diff.y < 0.0 ? -1 : 1;
    int stepZ = diff.z < 0.0 ? -1 : 1;
    int X = (int)floor(start.x);
    int Y = (int)floor(start.y);
    int Z = (int)floor(start.z);
    int limitX = (int)floor(end.x) + stepX;
    int limitY = (int)floor(end.y) + stepY;
    int limitZ = (int)floor(end.z) + stepZ;
    double tDeltaX = diff.x == 0.0 ? 0.0 : 1.0 / fabs(diff.x);
    double tDeltaY = diff.y == 0.0 ? 0.0 : 1.0 / fabs(diff.y);
    double tDeltaZ = diff.z == 0.0 ? 0.0 : 1.0 / fabs(diff.z);
    double tMaxX = diff.x == 0.0 ? INFINITY : ((double)(X + (stepX > 0 ? 1 : 0)) - start.x) / diff.x;
    double tMaxY = diff.y == 0.0 ? INFINITY : ((double)(Y + (stepY > 0 ? 1 : 0)) - start.y) / diff.y;
    double tMaxZ = diff.z == 0.0 ? INFINITY : ((double)(Z + (stepZ > 0 ? 1 : 0)) - start.z) / diff.z;

    do {
        if (not delegate.onCell(X, Y, Z)) {
            return false;
        }

        if (tMaxX < tMaxY) {
            if (tMaxX < tMaxZ) {
                X = X + stepX;
                tMaxX = tMaxX + tDeltaX;
            } else {
                Z = Z + stepZ;
                tMaxZ = tMaxZ + tDeltaZ;
            }
        } else {
            if (tMaxY < tMaxZ) {
                Y = Y + stepY;
                tMaxY = tMaxY + tDeltaY;
            } else {
                Z = Z + stepZ;
                tMaxZ = tMaxZ + tDeltaZ;
            }
        }
    } while (X != limitX and Y != limitY and Z != limitZ);

    return true;
}
