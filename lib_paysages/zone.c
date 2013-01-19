#include "zone.h"

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "tools.h"

#define MAX_CIRCLES 20

typedef struct
{
    double value;
    double centerx;
    double centerz;
    double softradius;
    double hardradius;
} Circle;

struct Zone {
    Curve* value_by_height;
    Curve* value_by_slope;

    Circle circles_included[MAX_CIRCLES];
    int circles_included_count;

    Circle circles_excluded[MAX_CIRCLES];
    int circles_excluded_count;
};

Zone* zoneCreate()
{
    Zone* result;

    result = (Zone*)malloc(sizeof(Zone));
    result->value_by_height = curveCreate();
    curveSetDefault(result->value_by_height, 1.0);
    result->value_by_slope = curveCreate();
    curveSetDefault(result->value_by_slope, 1.0);
    result->circles_included_count = 0;
    result->circles_excluded_count = 0;

    return result;
}

void zoneDelete(Zone* zone)
{
    curveDelete(zone->value_by_height);
    curveDelete(zone->value_by_slope);
    free(zone);
}

void zoneSave(PackStream* stream, Zone* zone)
{
    int i;

    curveSave(stream, zone->value_by_height);
    curveSave(stream, zone->value_by_slope);

    packWriteInt(stream, &zone->circles_included_count);
    for (i = 0; i < zone->circles_included_count; i++)
    {
        packWriteDouble(stream, &zone->circles_included[i].value);
        packWriteDouble(stream, &zone->circles_included[i].centerx);
        packWriteDouble(stream, &zone->circles_included[i].centerz);
        packWriteDouble(stream, &zone->circles_included[i].softradius);
        packWriteDouble(stream, &zone->circles_included[i].hardradius);
    }

    packWriteInt(stream, &zone->circles_excluded_count);
    for (i = 0; i < zone->circles_excluded_count; i++)
    {
        packWriteDouble(stream, &zone->circles_excluded[i].value);
        packWriteDouble(stream, &zone->circles_excluded[i].centerx);
        packWriteDouble(stream, &zone->circles_excluded[i].centerz);
        packWriteDouble(stream, &zone->circles_excluded[i].softradius);
        packWriteDouble(stream, &zone->circles_excluded[i].hardradius);
    }
}

void zoneLoad(PackStream* stream, Zone* zone)
{
    int i;

    curveLoad(stream, zone->value_by_height);
    curveLoad(stream, zone->value_by_slope);

    packReadInt(stream, &zone->circles_included_count);
    for (i = 0; i < zone->circles_included_count; i++)
    {
        packReadDouble(stream, &zone->circles_included[i].value);
        packReadDouble(stream, &zone->circles_included[i].centerx);
        packReadDouble(stream, &zone->circles_included[i].centerz);
        packReadDouble(stream, &zone->circles_included[i].softradius);
        packReadDouble(stream, &zone->circles_included[i].hardradius);
    }

    packReadInt(stream, &zone->circles_excluded_count);
    for (i = 0; i < zone->circles_excluded_count; i++)
    {
        packReadDouble(stream, &zone->circles_excluded[i].value);
        packReadDouble(stream, &zone->circles_excluded[i].centerx);
        packReadDouble(stream, &zone->circles_excluded[i].centerz);
        packReadDouble(stream, &zone->circles_excluded[i].softradius);
        packReadDouble(stream, &zone->circles_excluded[i].hardradius);
    }
}

void zoneCopy(Zone* source, Zone* destination)
{
    curveCopy(source->value_by_height, destination->value_by_height);
    curveCopy(source->value_by_slope, destination->value_by_slope);

    memcpy(destination->circles_included, source->circles_included, sizeof(Circle) * source->circles_included_count);
    destination->circles_included_count = source->circles_included_count;

    memcpy(destination->circles_excluded, source->circles_excluded, sizeof(Circle) * source->circles_excluded_count);
    destination->circles_excluded_count = source->circles_excluded_count;
}

void zoneIncludeCircleArea(Zone* zone, double value, double centerx, double centerz, double softradius, double hardradius)
{
    Circle circle = {value, centerx, centerz, softradius, hardradius};

    if (zone->circles_included_count < MAX_CIRCLES)
    {
        zone->circles_included[zone->circles_included_count++] = circle;
    }
}

void zoneExcludeCircleArea(Zone* zone, double centerx, double centerz, double softradius, double hardradius)
{
    /* TODO */
}

void zoneGetHeightCurve(Zone* zone, Curve* curve)
{
    curveCopy(zone->value_by_height, curve);
}

void zoneSetHeightCurve(Zone* zone, Curve* curve)
{
    curveCopy(curve, zone->value_by_height);
}

void zoneAddHeightRangeQuick(Zone* zone, double value, double hardmin, double softmin, double softmax, double hardmax)
{
    curveQuickAddPoint(zone->value_by_height, hardmin, 0.0);
    curveQuickAddPoint(zone->value_by_height, softmin, value);
    curveQuickAddPoint(zone->value_by_height, softmax, value);
    curveQuickAddPoint(zone->value_by_height, hardmax, 0.0);
}

void zoneGetSlopeCurve(Zone* zone, Curve* curve)
{
    curveCopy(zone->value_by_slope, curve);
}

void zoneSetSlopeCurve(Zone* zone, Curve* curve)
{
    curveCopy(curve, zone->value_by_slope);
}

void zoneAddSlopeRangeQuick(Zone* zone, double value, double hardmin, double softmin, double softmax, double hardmax)
{
    curveQuickAddPoint(zone->value_by_slope, hardmin, 0.0);
    curveQuickAddPoint(zone->value_by_slope, softmin, value);
    curveQuickAddPoint(zone->value_by_slope, softmax, value);
    curveQuickAddPoint(zone->value_by_slope, hardmax, 0.0);
}

static inline double _getCircleInfluence(Circle circle, Vector3 position)
{
    double radius, dx, dz;

    dx = position.x - circle.centerx;
    dz = position.z - circle.centerz;
    radius = sqrt(dx * dx + dz * dz);

    if (radius > circle.hardradius)
    {
        return 0.0;
    }
    else if (radius < circle.softradius)
    {
        return circle.value;
    }
    else
    {
        return circle.value * (circle.hardradius - radius) / (circle.hardradius - circle.softradius);
    }
}

double zoneGetValue(Zone* zone, Vector3 location, Vector3 normal)
{
    int i;
    double value, value_height, value_steepness, value_circle;

    if (zone->circles_included_count > 0)
    {
        value_circle = 0.0;
        for (i = 0; i < zone->circles_included_count; i++)
        {
            value = _getCircleInfluence(zone->circles_included[i], location);
            if (value > value_circle)
            {
                value_circle = value;
            }
        }
    }
    else
    {
        value_circle = 1.0;
    }

    value_height = curveGetValue(zone->value_by_height, location.y);
    value_steepness = curveGetValue(zone->value_by_slope, (1.0 - normal.y));

    if (value_steepness < value_height)
    {
        if (value_circle < value_steepness)
        {
            return value_circle;
        }
        else
        {
            return value_steepness;
        }
    }
    else
    {
        if (value_circle < value_height)
        {
            return value_circle;
        }
        else
        {
            return value_height;
        }
    }
}

