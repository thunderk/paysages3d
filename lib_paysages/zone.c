#include "zone.h"

#include <stdlib.h>
#include <math.h>
#include "tools.h"

#define MAX_RANGES 20
#define MAX_CIRCLES 20

typedef struct
{
    float value;
    float centerx;
    float centerz;
    float softradius;
    float hardradius;
} Circle;

struct Zone {
    ZoneRangeCondition height_ranges[MAX_RANGES];
    int height_ranges_count;

    ZoneRangeCondition slope_ranges[MAX_RANGES];
    int slope_ranges_count;

    Circle circles_included[MAX_CIRCLES];
    int circles_included_count;

    Circle circles_excluded[MAX_CIRCLES];
    int circles_excluded_count;
};

Zone* zoneCreate()
{
    Zone* result;

    result = (Zone*)malloc(sizeof(Zone));
    result->height_ranges_count = 0;
    result->slope_ranges_count = 0;
    result->circles_included_count = 0;
    result->circles_excluded_count = 0;

    return result;
}

void zoneDelete(Zone* zone)
{
    free(zone);
}

void zoneSave(PackStream* stream, Zone* zone)
{
    int i;

    packWriteInt(stream, &zone->height_ranges_count);
    for (i = 0; i < zone->height_ranges_count; i++)
    {
        packWriteFloat(stream, &zone->height_ranges[i].value);
        packWriteFloat(stream, &zone->height_ranges[i].hardmin);
        packWriteFloat(stream, &zone->height_ranges[i].softmin);
        packWriteFloat(stream, &zone->height_ranges[i].softmax);
        packWriteFloat(stream, &zone->height_ranges[i].hardmax);
    }

    packWriteInt(stream, &zone->slope_ranges_count);
    for (i = 0; i < zone->slope_ranges_count; i++)
    {
        packWriteFloat(stream, &zone->slope_ranges[i].value);
        packWriteFloat(stream, &zone->slope_ranges[i].hardmin);
        packWriteFloat(stream, &zone->slope_ranges[i].softmin);
        packWriteFloat(stream, &zone->slope_ranges[i].softmax);
        packWriteFloat(stream, &zone->slope_ranges[i].hardmax);
    }

    packWriteInt(stream, &zone->circles_included_count);
    for (i = 0; i < zone->circles_included_count; i++)
    {
        packWriteFloat(stream, &zone->circles_included[i].value);
        packWriteFloat(stream, &zone->circles_included[i].centerx);
        packWriteFloat(stream, &zone->circles_included[i].centerz);
        packWriteFloat(stream, &zone->circles_included[i].softradius);
        packWriteFloat(stream, &zone->circles_included[i].hardradius);
    }

    packWriteInt(stream, &zone->circles_excluded_count);
    for (i = 0; i < zone->circles_excluded_count; i++)
    {
        packWriteFloat(stream, &zone->circles_excluded[i].value);
        packWriteFloat(stream, &zone->circles_excluded[i].centerx);
        packWriteFloat(stream, &zone->circles_excluded[i].centerz);
        packWriteFloat(stream, &zone->circles_excluded[i].softradius);
        packWriteFloat(stream, &zone->circles_excluded[i].hardradius);
    }
}

void zoneLoad(PackStream* stream, Zone* zone)
{
    int i;

    packReadInt(stream, &zone->height_ranges_count);
    for (i = 0; i < zone->height_ranges_count; i++)
    {
        packReadFloat(stream, &zone->height_ranges[i].value);
        packReadFloat(stream, &zone->height_ranges[i].hardmin);
        packReadFloat(stream, &zone->height_ranges[i].softmin);
        packReadFloat(stream, &zone->height_ranges[i].softmax);
        packReadFloat(stream, &zone->height_ranges[i].hardmax);
    }

    packReadInt(stream, &zone->slope_ranges_count);
    for (i = 0; i < zone->slope_ranges_count; i++)
    {
        packReadFloat(stream, &zone->slope_ranges[i].value);
        packReadFloat(stream, &zone->slope_ranges[i].hardmin);
        packReadFloat(stream, &zone->slope_ranges[i].softmin);
        packReadFloat(stream, &zone->slope_ranges[i].softmax);
        packReadFloat(stream, &zone->slope_ranges[i].hardmax);
    }

    packReadInt(stream, &zone->circles_included_count);
    for (i = 0; i < zone->circles_included_count; i++)
    {
        packReadFloat(stream, &zone->circles_included[i].value);
        packReadFloat(stream, &zone->circles_included[i].centerx);
        packReadFloat(stream, &zone->circles_included[i].centerz);
        packReadFloat(stream, &zone->circles_included[i].softradius);
        packReadFloat(stream, &zone->circles_included[i].hardradius);
    }

    packReadInt(stream, &zone->circles_excluded_count);
    for (i = 0; i < zone->circles_excluded_count; i++)
    {
        packReadFloat(stream, &zone->circles_excluded[i].value);
        packReadFloat(stream, &zone->circles_excluded[i].centerx);
        packReadFloat(stream, &zone->circles_excluded[i].centerz);
        packReadFloat(stream, &zone->circles_excluded[i].softradius);
        packReadFloat(stream, &zone->circles_excluded[i].hardradius);
    }
}

void zoneCopy(Zone* source, Zone* destination)
{
    *destination = *source;
}

void zoneIncludeCircleArea(Zone* zone, float value, float centerx, float centerz, float softradius, float hardradius)
{
    Circle circle = {value, centerx, centerz, softradius, hardradius};

    if (zone->circles_included_count < MAX_CIRCLES)
    {
        zone->circles_included[zone->circles_included_count++] = circle;
    }
}

void zoneExcludeCircleArea(Zone* zone, float centerx, float centerz, float softradius, float hardradius)
{
    /* TODO */
}

int zoneAddHeightRange(Zone* zone)
{
    if (zone->height_ranges_count < MAX_RANGES)
    {
        zone->height_ranges[zone->height_ranges_count].value = 0.0;
        zone->height_ranges[zone->height_ranges_count].softmin = 0.0;
        zone->height_ranges[zone->height_ranges_count].hardmin = 0.0;
        zone->height_ranges[zone->height_ranges_count].hardmax = 0.0;
        zone->height_ranges[zone->height_ranges_count].softmax = 0.0;
        return zone->height_ranges_count++;
    }
    else
    {
        return -1;
    }
}

int zoneGetHeightRangeCount(Zone* zone)
{
    return zone->height_ranges_count;
}

int zoneGetHeightRange(Zone* zone, int position, ZoneRangeCondition* range)
{
    if (position >= 0 && position < zone->height_ranges_count)
    {
        *range = zone->height_ranges[position];
        return 1;
    }
    else
    {
        return 0;
    }
}

int zoneSetHeightRange(Zone* zone, int position, ZoneRangeCondition* range)
{
    if (position >= 0 && position < zone->height_ranges_count)
    {
        zone->height_ranges[position] = *range;
        return 1;
    }
    else
    {
        return 0;
    }
}

int zoneAddHeightRangeQuick(Zone* zone, float value, float hardmin, float softmin, float softmax, float hardmax)
{
    ZoneRangeCondition range = {value, hardmin, softmin, softmax, hardmax};
    int position;
    
    position = zoneAddHeightRange(zone);
    if (position >= 0)
    {
        zoneSetHeightRange(zone, position, &range);
    }
    return position;
}

int zoneAddSlopeRange(Zone* zone)
{
    if (zone->slope_ranges_count < MAX_RANGES)
    {
        zone->slope_ranges[zone->slope_ranges_count].value = 0.0;
        zone->slope_ranges[zone->slope_ranges_count].softmin = 0.0;
        zone->slope_ranges[zone->slope_ranges_count].hardmin = 0.0;
        zone->slope_ranges[zone->slope_ranges_count].hardmax = 0.0;
        zone->slope_ranges[zone->slope_ranges_count].softmax = 0.0;
        return zone->slope_ranges_count++;
    }
    else
    {
        return -1;
    }
}

int zoneGetSlopeRangeCount(Zone* zone)
{
    return zone->slope_ranges_count;
}

int zoneGetSlopeRange(Zone* zone, int position, ZoneRangeCondition* range)
{
    if (position >= 0 && position < zone->slope_ranges_count)
    {
        *range = zone->slope_ranges[position];
        return 1;
    }
    else
    {
        return 0;
    }
}

int zoneSetSlopeRange(Zone* zone, int position, ZoneRangeCondition* range)
{
    if (position >= 0 && position < zone->slope_ranges_count)
    {
        zone->slope_ranges[position] = *range;
        return 1;
    }
    else
    {
        return 0;
    }
}

int zoneAddSlopeRangeQuick(Zone* zone, float value, float hardmin, float softmin, float softmax, float hardmax)
{
    ZoneRangeCondition range = {value, hardmin, softmin, softmax, hardmax};
    int position;
    
    position = zoneAddSlopeRange(zone);
    if (position >= 0)
    {
        zoneSetSlopeRange(zone, position, &range);
    }
    return position;
}

static inline float _getRangeInfluence(ZoneRangeCondition range, float position)
{
    if (position >= range.hardmin && position <= range.hardmax)
    {
        if (position < range.softmin)
        {
            return range.value * (position - range.hardmin) / (range.softmin - range.hardmin);
        }
        else if (position > range.softmax)
        {
            return range.value * (range.hardmax - position) / (range.hardmax - range.softmax);
        }
        else
        {
            return range.value;
        }
    }
    else
    {
        return 0.0;
    }
}

static inline float _getCircleInfluence(Circle circle, Vector3 position)
{
    float radius, dx, dz;

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

float zoneGetValue(Zone* zone, Vector3 location, Vector3 normal)
{
    int i;
    float value, value_height, value_steepness, value_circle;

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

    if (zone->height_ranges_count > 0)
    {
        value_height = 0.0;
        for (i = 0; i < zone->height_ranges_count; i++)
        {
            value = _getRangeInfluence(zone->height_ranges[i], location.y);
            if (value > value_height)
            {
                value_height = value;
            }
        }
    }
    else
    {
        value_height = 1.0;
    }

    if (zone->slope_ranges_count > 0)
    {
        value_steepness = 0.0;
        for (i = 0; i < zone->slope_ranges_count; i++)
        {
            value = _getRangeInfluence(zone->slope_ranges[i], (1.0 - normal.y));
            if (value > value_steepness)
            {
                value_steepness = value;
            }
        }
    }
    else
    {
        value_steepness = 1.0;
    }

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

