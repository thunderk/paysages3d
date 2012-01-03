#include <stdlib.h>
#include <math.h>

#define MAX_RANGES 20
#define MAX_CIRCLES 20

typedef struct
{
    double value;
    double hardmin;
    double softmin;
    double softmax;
    double hardmax;
} Range;

typedef struct
{
    double value;
    double centerx;
    double centerz;
    double softradius;
    double hardradius;
} Circle;

struct Zone {
    Range height_ranges[MAX_RANGES];
    int height_ranges_count;

    Range steepness_ranges[MAX_RANGES];
    int steepness_ranges_count;

    Circle circles_included[MAX_CIRCLES];
    int circles_included_count;

    Circle circles_excluded[MAX_CIRCLES];
    int circles_excluded_count;
};

#include "shared/types.h"
#include "shared/functions.h"

Zone* zoneCreate()
{
    Zone* result;

    result = (Zone*)malloc(sizeof(Zone));
    result->height_ranges_count = 0;
    result->steepness_ranges_count = 0;
    result->circles_included_count = 0;
    result->circles_excluded_count = 0;
    
    return result;
}

void zoneDelete(Zone* zone)
{
    free(zone);
}

void zoneSave(Zone* zone, FILE* f)
{
    int i;
    
    toolsSaveInt(f, zone->height_ranges_count);
    for (i = 0; i < zone->height_ranges_count; i++)
    {
        toolsSaveDouble(f, zone->height_ranges[i].value);
        toolsSaveDouble(f, zone->height_ranges[i].hardmin);
        toolsSaveDouble(f, zone->height_ranges[i].softmin);
        toolsSaveDouble(f, zone->height_ranges[i].softmax);
        toolsSaveDouble(f, zone->height_ranges[i].hardmax);
    }
    
    toolsSaveInt(f, zone->steepness_ranges_count);
    for (i = 0; i < zone->steepness_ranges_count; i++)
    {
        toolsSaveDouble(f, zone->steepness_ranges[i].value);
        toolsSaveDouble(f, zone->steepness_ranges[i].hardmin);
        toolsSaveDouble(f, zone->steepness_ranges[i].softmin);
        toolsSaveDouble(f, zone->steepness_ranges[i].softmax);
        toolsSaveDouble(f, zone->steepness_ranges[i].hardmax);
    }
    
    toolsSaveInt(f, zone->circles_included_count);
    for (i = 0; i < zone->circles_included_count; i++)
    {
        toolsSaveDouble(f, zone->circles_included[i].value);
        toolsSaveDouble(f, zone->circles_included[i].centerx);
        toolsSaveDouble(f, zone->circles_included[i].centerz);
        toolsSaveDouble(f, zone->circles_included[i].softradius);
        toolsSaveDouble(f, zone->circles_included[i].hardradius);
    }
    
    toolsSaveInt(f, zone->circles_excluded_count);
    for (i = 0; i < zone->circles_excluded_count; i++)
    {
        toolsSaveDouble(f, zone->circles_excluded[i].value);
        toolsSaveDouble(f, zone->circles_excluded[i].centerx);
        toolsSaveDouble(f, zone->circles_excluded[i].centerz);
        toolsSaveDouble(f, zone->circles_excluded[i].softradius);
        toolsSaveDouble(f, zone->circles_excluded[i].hardradius);
    }
}

void zoneLoad(Zone* zone, FILE* f)
{
    int i;
    
    zone->height_ranges_count = toolsLoadInt(f);
    for (i = 0; i < zone->height_ranges_count; i++)
    {
        zone->height_ranges[i].value = toolsLoadDouble(f);
        zone->height_ranges[i].hardmin = toolsLoadDouble(f);
        zone->height_ranges[i].softmin = toolsLoadDouble(f);
        zone->height_ranges[i].softmax = toolsLoadDouble(f);
        zone->height_ranges[i].hardmax = toolsLoadDouble(f);
    }
    
    zone->steepness_ranges_count = toolsLoadInt(f);
    for (i = 0; i < zone->steepness_ranges_count; i++)
    {
        zone->steepness_ranges[i].value = toolsLoadDouble(f);
        zone->steepness_ranges[i].hardmin = toolsLoadDouble(f);
        zone->steepness_ranges[i].softmin = toolsLoadDouble(f);
        zone->steepness_ranges[i].softmax = toolsLoadDouble(f);
        zone->steepness_ranges[i].hardmax = toolsLoadDouble(f);
    }
    
    zone->circles_included_count = toolsLoadInt(f);
    for (i = 0; i < zone->circles_included_count; i++)
    {
        zone->circles_included[i].value = toolsLoadDouble(f);
        zone->circles_included[i].centerx = toolsLoadDouble(f);
        zone->circles_included[i].centerz = toolsLoadDouble(f);
        zone->circles_included[i].softradius = toolsLoadDouble(f);
        zone->circles_included[i].hardradius = toolsLoadDouble(f);
    }
    
    zone->circles_excluded_count = toolsLoadInt(f);
    for (i = 0; i < zone->circles_excluded_count; i++)
    {
        zone->circles_excluded[i].value = toolsLoadDouble(f);
        zone->circles_excluded[i].centerx = toolsLoadDouble(f);
        zone->circles_excluded[i].centerz = toolsLoadDouble(f);
        zone->circles_excluded[i].softradius = toolsLoadDouble(f);
        zone->circles_excluded[i].hardradius = toolsLoadDouble(f);
    }
}

void zoneCopy(Zone* source, Zone* destination)
{
    *source = *destination;
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

void zoneAddHeightRange(Zone* zone, double value, double hardmin, double softmin, double softmax, double hardmax)
{
    Range range = {value, hardmin, softmin, softmax, hardmax};

    if (zone->height_ranges_count < MAX_RANGES)
    {
        zone->height_ranges[zone->height_ranges_count++] = range;
    }
}

void zoneAddSteepnessRange(Zone* zone, double value, double hardmin, double softmin, double softmax, double hardmax)
{
    Range range = {value, hardmin, softmin, softmax, hardmax};

    if (zone->steepness_ranges_count < MAX_RANGES)
    {
        zone->steepness_ranges[zone->steepness_ranges_count++] = range;
    }
}

static inline double _getRangeInfluence(Range range, double position)
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

    if (zone->steepness_ranges_count > 0)
    {
        value_steepness = 0.0;
        for (i = 0; i < zone->steepness_ranges_count; i++)
        {
            value = _getRangeInfluence(zone->steepness_ranges[i], (1.0 - normal.y));
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

