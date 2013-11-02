#include "boundingbox.h"

void boundingBoxReset(BoundingBox* box)
{
    box->empty = 1;
    box->xmin = 10000000000.0;
    box->xmax = -10000000000.0;
    box->ymin = 10000000000.0;
    box->ymax = -10000000000.0;
    box->zmin = 10000000000.0;
    box->zmax = -10000000000.0;
}

void boundingBoxPushPoint(BoundingBox* box, Vector3 point)
{
    box->empty = 0;
    if (point.x < box->xmin)
    {
        box->xmin = point.x;
    }
    if (point.x > box->xmax)
    {
        box->xmax = point.x;
    }
    if (point.y < box->ymin)
    {
        box->ymin = point.y;
    }
    if (point.y > box->ymax)
    {
        box->ymax = point.y;
    }
    if (point.z < box->zmin)
    {
        box->zmin = point.z;
    }
    if (point.z > box->zmax)
    {
        box->zmax = point.z;
    }
}
