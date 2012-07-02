#include "curve.h"

#include <stdlib.h>
#include <string.h>
#include "tools.h"

#define MAX_NB_POINTS 40

struct Curve
{
    double default_value;
    int nbpoints;
    CurvePoint points[MAX_NB_POINTS];
};

Curve* curveCreate()
{
    Curve* result;
    
    result = malloc(sizeof(Curve));
    result->nbpoints = 0;
    result->default_value = 0.0;
    
    return result;
}

void curveDelete(Curve* curve)
{
    free(curve);
}

void curveCopy(Curve* source, Curve* destination)
{
    *destination = *source;
}

void curveSave(PackStream* stream, Curve* curve)
{
    int i;
    
    packWriteDouble(stream, &curve->default_value);
    packWriteInt(stream, &curve->nbpoints);
    for (i = 0; i < curve->nbpoints; i++)
    {
        packWriteDouble(stream, &curve->points[i].position);
        packWriteDouble(stream, &curve->points[i].value);
    }
}

void curveLoad(PackStream* stream, Curve* curve)
{
    int i;
    
    packReadDouble(stream, &curve->default_value);
    packReadInt(stream, &curve->nbpoints);
    for (i = 0; i < curve->nbpoints; i++)
    {
        packReadDouble(stream, &curve->points[i].position);
        packReadDouble(stream, &curve->points[i].value);
    }
}

void curveClear(Curve* curve)
{
    curve->nbpoints = 0;
}

void curveSetDefault(Curve* curve, double value)
{
    curve->default_value = value;
}

int curveAddPoint(Curve* curve, CurvePoint* point)
{
    if (curve->nbpoints < MAX_NB_POINTS)
    {
        curve->points[curve->nbpoints] = *point;
        return curve->nbpoints++;
    }
    else
    {
        return -1;
    }
}

int curveQuickAddPoint(Curve* curve, double position, double value)
{
    CurvePoint point;
    
    point.position = position;
    point.value = value;
    
    return curveAddPoint(curve, &point);
}

int curveGetPointCount(Curve* curve)
{
    return curve->nbpoints;
}

void curveGetPoint(Curve* curve, int number, CurvePoint* point)
{
    if (number >= 0 && number < curve->nbpoints)
    {
        *point = curve->points[number];
    }
}

void curveSetPoint(Curve* curve, int number, CurvePoint* point)
{
    if (number >= 0 && number < curve->nbpoints)
    {
        curve->points[number] = *point;
    }
}

void curveRemovePoint(Curve* curve, int number)
{
    if (number >= 0 && number < curve->nbpoints)
    {
        if (curve->nbpoints > 0 && number < curve->nbpoints - 1)
        {
            memmove(curve->points + number, curve->points + number + 1, sizeof(CurvePoint) * (curve->nbpoints - number - 1));
        }
        curve->nbpoints--;
    }
}

int _point_compare(const void* part1, const void* part2)
{
    if (((CurvePoint*)part1)->position > ((CurvePoint*)part2)->position)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

void curveValidate(Curve* curve)
{
    if (curve->nbpoints > 1)
    {
        qsort(curve->points, curve->nbpoints, sizeof(CurvePoint), _point_compare);
    }
}

double curveGetValue(Curve* curve, double position)
{
    int i;
    double fact;

    if (curve->nbpoints == 0)
    {
        return curve->default_value;
    }
    else if (curve->nbpoints == 1 || position <= curve->points[0].position)
    {
        return curve->points[0].value;
    }
    else if (position >= curve->points[curve->nbpoints - 1].position)
    {
        return curve->points[curve->nbpoints - 1].value;
    }
    else
    {
        for (i = 1; i < curve->nbpoints; i++)
        {
            if (position < curve->points[i].position)
            {
                fact = (position - curve->points[i - 1].position) / (curve->points[i].position - curve->points[i - 1].position);
                return curve->points[i - 1].value + (curve->points[i].value - curve->points[i - 1].value) * fact;
            }
        }
        return curve->points[curve->nbpoints - 1].value;
    }
}
