#include "Curve.h"

#include <cstring>
#include "PackStream.h"

const int MAX_NB_POINTS = 30;

Curve::Curve()
{
    nbpoints = 0;
    default_value = 0.0;
    points = new CurvePoint[MAX_NB_POINTS];
}

Curve::~Curve()
{
    delete[] points;
}

void Curve::copy(Curve* destination) const
{
    destination->nbpoints = nbpoints;
    destination->default_value = default_value;
    for (int i = 0; i < nbpoints; i++)
    {
        destination->points[i] = points[i];
    }
}

void Curve::save(PackStream* stream) const
{
    stream->write(&default_value);
    stream->write(&nbpoints);
    for (int i = 0; i < nbpoints; i++)
    {
        stream->write(&points[i].position);
        stream->write(&points[i].value);
    }
}

void Curve::load(PackStream* stream)
{
    stream->read(&default_value);
    stream->read(&nbpoints);
    for (int i = 0; i < nbpoints; i++)
    {
        stream->read(&points[i].position);
        stream->read(&points[i].value);
    }
}

void Curve::clear()
{
    nbpoints = 0;
}

void Curve::setDefault(double value)
{
    default_value = value;
}

int Curve::addPoint(const CurvePoint &point)
{
    if (nbpoints < MAX_NB_POINTS)
    {
        points[nbpoints] = point;
        return nbpoints++;
    }
    else
    {
        return -1;
    }
}

int Curve::addPoint(double position, double value)
{
    CurvePoint point = {position, value};
    return addPoint(point);
}

CurvePoint Curve::getPoint(int number) const
{
    if (number >= 0 && number < nbpoints)
    {
        return points[number];
    }
    else
    {
        return {0.0, 0.0};
    }
}

bool Curve::getPoint(int number, CurvePoint *point) const
{
    if (number >= 0 && number < nbpoints)
    {
        *point = points[number];
        return true;
    }
    else
    {
        return false;
    }
}

void Curve::setPoint(int number, const CurvePoint &point)
{
    if (number >= 0 && number < nbpoints)
    {
        points[number] = point;
    }
}

void Curve::removePoint(int number)
{
    if (number >= 0 && number < nbpoints)
    {
        if (nbpoints > 0 && number < nbpoints - 1)
        {
            memmove(points + number, points + number + 1, sizeof(CurvePoint) * (nbpoints - number - 1));
        }
        nbpoints--;
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

void Curve::validate()
{
    if (nbpoints > 1)
    {
        qsort(points, nbpoints, sizeof(CurvePoint), _point_compare);
    }
}

double Curve::getValue(double position) const
{
    int i;
    double fact;

    if (nbpoints == 0)
    {
        return default_value;
    }
    else if (nbpoints == 1 || position <= points[0].position)
    {
        return points[0].value;
    }
    else if (position >= points[nbpoints - 1].position)
    {
        return points[nbpoints - 1].value;
    }
    else
    {
        for (i = 1; i < nbpoints; i++)
        {
            if (position < points[i].position)
            {
                fact = (position - points[i - 1].position) / (points[i].position - points[i - 1].position);
                return points[i - 1].value + (points[i].value - points[i - 1].value) * fact;
            }
        }
        return points[nbpoints - 1].value;
    }
}
