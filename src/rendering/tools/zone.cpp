#include "zone.h"

#include <cstring>
#include <cstdlib>
#include <cmath>
#include "PackStream.h"
#include "Curve.h"
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
    int absolute_height;
    double relative_height_min;
    double relative_height_middle;
    double relative_height_max;

    Curve* value_by_height;
    Curve* value_by_slope;

    Circle circles_included[MAX_CIRCLES];
    int circles_included_count;
};

Zone* zoneCreate()
{
    Zone* result;

    result = (Zone*)malloc(sizeof(Zone));
    result->value_by_height = new Curve;
    result->absolute_height = 1;
    result->value_by_height->setDefault(1.0);
    result->value_by_slope = new Curve;
    result->value_by_slope->setDefault(1.0);
    result->circles_included_count = 0;

    return result;
}

void zoneDelete(Zone* zone)
{
    delete zone->value_by_height;
    delete zone->value_by_slope;
    free(zone);
}

void zoneSave(PackStream* stream, Zone* zone)
{
    int i;

    stream->write(&zone->absolute_height);
    stream->write(&zone->relative_height_min);
    stream->write(&zone->relative_height_middle);
    stream->write(&zone->relative_height_max);

    zone->value_by_height->save(stream);
    zone->value_by_slope->save(stream);

    stream->write(&zone->circles_included_count);
    for (i = 0; i < zone->circles_included_count; i++)
    {
        stream->write(&zone->circles_included[i].value);
        stream->write(&zone->circles_included[i].centerx);
        stream->write(&zone->circles_included[i].centerz);
        stream->write(&zone->circles_included[i].softradius);
        stream->write(&zone->circles_included[i].hardradius);
    }
}

void zoneLoad(PackStream* stream, Zone* zone)
{
    int i;

    stream->read(&zone->absolute_height);
    stream->read(&zone->relative_height_min);
    stream->read(&zone->relative_height_middle);
    stream->read(&zone->relative_height_max);

    zone->value_by_height->load(stream);
    zone->value_by_slope->load(stream);

    stream->read(&zone->circles_included_count);
    for (i = 0; i < zone->circles_included_count; i++)
    {
        stream->read(&zone->circles_included[i].value);
        stream->read(&zone->circles_included[i].centerx);
        stream->read(&zone->circles_included[i].centerz);
        stream->read(&zone->circles_included[i].softradius);
        stream->read(&zone->circles_included[i].hardradius);
    }
}

void zoneCopy(Zone* source, Zone* destination)
{
    destination->absolute_height = source->absolute_height;
    destination->relative_height_min = source->relative_height_min;
    destination->relative_height_middle = source->relative_height_middle;
    destination->relative_height_max = source->relative_height_max;

    source->value_by_height->copy(destination->value_by_height);
    source->value_by_slope->copy(destination->value_by_slope);

    memcpy(destination->circles_included, source->circles_included, sizeof(Circle) * source->circles_included_count);
    destination->circles_included_count = source->circles_included_count;
}

void zoneClear(Zone* zone)
{
    zone->value_by_height->clear();
    zone->value_by_slope->clear();
    zone->circles_included_count = 0;
}

void zoneSetAbsoluteHeight(Zone* zone)
{
    zone->absolute_height = 1;
}

void zoneSetRelativeHeight(Zone* zone, double min, double middle, double max)
{
    if (max < min)
    {
        max = min;
    }
    if (middle < min)
    {
        middle = min;
    }
    if (middle > max)
    {
        middle = max;
    }

    zone->absolute_height = 0;
    zone->relative_height_min = min;
    zone->relative_height_middle = middle;
    zone->relative_height_max = max;
}

void zoneIncludeCircleArea(Zone* zone, double value, double centerx, double centerz, double softradius, double hardradius)
{
    Circle circle = {value, centerx, centerz, softradius, hardradius};

    if (zone->circles_included_count < MAX_CIRCLES)
    {
        zone->circles_included[zone->circles_included_count++] = circle;
    }
}

void zoneGetHeightCurve(Zone* zone, Curve* curve)
{
    zone->value_by_height->copy(curve);
}

void zoneSetHeightCurve(Zone* zone, Curve* curve)
{
    curve->copy(zone->value_by_height);
}

void zoneAddHeightRangeQuick(Zone* zone, double value, double hardmin, double softmin, double softmax, double hardmax)
{
    zone->value_by_height->addPoint(hardmin, 0.0);
    zone->value_by_height->addPoint(softmin, value);
    zone->value_by_height->addPoint(softmax, value);
    zone->value_by_height->addPoint(hardmax, 0.0);
}

void zoneGetSlopeCurve(Zone* zone, Curve* curve)
{
    zone->value_by_slope->copy(curve);
}

void zoneSetSlopeCurve(Zone* zone, Curve* curve)
{
    curve->copy(zone->value_by_slope);
}

void zoneAddSlopeRangeQuick(Zone* zone, double value, double hardmin, double softmin, double softmax, double hardmax)
{
    zone->value_by_slope->addPoint(hardmin, 0.0);
    zone->value_by_slope->addPoint(softmin, value);
    zone->value_by_slope->addPoint(softmax, value);
    zone->value_by_slope->addPoint(hardmax, 0.0);
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
    double final_height;
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

    if (zone->absolute_height)
    {
        final_height = location.y;
    }
    else
    {
        if (location.y >= zone->relative_height_max)
        {
            final_height = 1.0;
        }
        else if (location.y <= zone->relative_height_min)
        {
            final_height = 0.0;
        }
        else if (location.y <= zone->relative_height_middle)
        {
            final_height = 0.5 * (location.y - zone->relative_height_min) / (zone->relative_height_middle - zone->relative_height_min);
        }
        else
        {
            final_height = 0.5 + 0.5 * (location.y - zone->relative_height_middle) / (zone->relative_height_max - zone->relative_height_middle);
        }
    }

    value_height = zone->value_by_height->getValue(final_height);
    value_steepness = zone->value_by_slope->getValue(1.0 - normal.y);

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

