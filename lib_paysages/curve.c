#include "curve.h"

#include <stdlib.h>
#include "tools.h"

#define MAX_NB_POINTS 40

struct Curve
{
    int nbpoints;
    CurvePoint points[MAX_NB_POINTS];
};

Curve* curveCreate()
{
    Curve* result;
    
    result = malloc(sizeof(Curve));
    result->nbpoints = 0;
    
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

void curveSave(FILE* f, Curve* curve)
{
    int i;
    
    toolsSaveInt(f, &curve->nbpoints);
    for (i = 0; i < curve->nbpoints; i++)
    {
        toolsSaveDouble(f, &curve->points[i].position);
        toolsSaveDouble(f, &curve->points[i].value);
    }
}

void curveLoad(FILE* f, Curve* curve)
{
    int i;
    
    toolsLoadInt(f, &curve->nbpoints);
    for (i = 0; i < curve->nbpoints; i++)
    {
        toolsLoadDouble(f, &curve->points[i].position);
        toolsLoadDouble(f, &curve->points[i].value);
    }
}

void curveClear(Curve* curve)
{
    curve->nbpoints = 0;
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
        return 0.0;
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
