#include "Zone.h"

#include <cstring>
#include "Curve.h"
#include "PackStream.h"
#include "Vector3.h"

#define MAX_CIRCLES 10

namespace paysages {
namespace definition {
class Circle
{
public:
    double value;
    double centerx;
    double centerz;
    double softradius;
    double hardradius;
};
}
}

Zone::Zone(BaseDefinition *parent):
    BaseDefinition(parent)
{
    value_by_height = new Curve;
    absolute_height = 1;
    value_by_height->setDefault(1.0);
    value_by_slope = new Curve;
    value_by_slope->setDefault(1.0);
    circles_included_count = 0;
    circles_included = new Circle[MAX_CIRCLES];
}

Zone::~Zone()
{
    delete value_by_height;
    delete value_by_slope;
    delete circles_included;
}

void Zone::save(PackStream* stream) const
{
    int i;

    stream->write(&absolute_height);
    stream->write(&relative_height_min);
    stream->write(&relative_height_middle);
    stream->write(&relative_height_max);

    value_by_height->save(stream);
    value_by_slope->save(stream);

    stream->write(&circles_included_count);
    for (i = 0; i < circles_included_count; i++)
    {
        stream->write(&circles_included[i].value);
        stream->write(&circles_included[i].centerx);
        stream->write(&circles_included[i].centerz);
        stream->write(&circles_included[i].softradius);
        stream->write(&circles_included[i].hardradius);
    }
}

void Zone::load(PackStream* stream)
{
    int i;

    stream->read(&absolute_height);
    stream->read(&relative_height_min);
    stream->read(&relative_height_middle);
    stream->read(&relative_height_max);

    value_by_height->load(stream);
    value_by_slope->load(stream);

    stream->read(&circles_included_count);
    for (i = 0; i < circles_included_count; i++)
    {
        stream->read(&circles_included[i].value);
        stream->read(&circles_included[i].centerx);
        stream->read(&circles_included[i].centerz);
        stream->read(&circles_included[i].softradius);
        stream->read(&circles_included[i].hardradius);
    }
}

void Zone::copy(BaseDefinition* _destination) const
{
    Zone* destination = (Zone*)_destination;

    destination->absolute_height = absolute_height;
    destination->relative_height_min = relative_height_min;
    destination->relative_height_middle = relative_height_middle;
    destination->relative_height_max = relative_height_max;

    value_by_height->copy(destination->value_by_height);
    value_by_slope->copy(destination->value_by_slope);

    memcpy(destination->circles_included, circles_included, sizeof(Circle) * circles_included_count);
    destination->circles_included_count = circles_included_count;
}

void Zone::clear()
{
    value_by_height->clear();
    value_by_slope->clear();
    circles_included_count = 0;
}

void Zone::setAbsoluteHeight()
{
    absolute_height = 1;
}

void Zone::setRelativeHeight(double min, double middle, double max)
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

    absolute_height = 0;
    relative_height_min = min;
    relative_height_middle = middle;
    relative_height_max = max;
}

void Zone::includeCircleArea(double value, double centerx, double centerz, double softradius, double hardradius)
{
    Circle circle = {value, centerx, centerz, softradius, hardradius};

    if (circles_included_count < MAX_CIRCLES)
    {
        circles_included[circles_included_count++] = circle;
    }
}

void Zone::getHeightCurve(Curve* curve) const
{
    value_by_height->copy(curve);
}

void Zone::setHeightCurve(Curve* curve)
{
    curve->copy(value_by_height);
}

void Zone::addHeightRangeQuick(double value, double hardmin, double softmin, double softmax, double hardmax)
{
    value_by_height->addPoint(hardmin, 0.0);
    value_by_height->addPoint(softmin, value);
    value_by_height->addPoint(softmax, value);
    value_by_height->addPoint(hardmax, 0.0);
}

void Zone::getSlopeCurve(Curve* curve) const
{
    value_by_slope->copy(curve);
}

void Zone::setSlopeCurve(Curve* curve)
{
    curve->copy(value_by_slope);
}

void Zone::addSlopeRangeQuick(double value, double hardmin, double softmin, double softmax, double hardmax)
{
    value_by_slope->addPoint(hardmin, 0.0);
    value_by_slope->addPoint(softmin, value);
    value_by_slope->addPoint(softmax, value);
    value_by_slope->addPoint(hardmax, 0.0);
}

static inline double _getCircleInfluence(const Circle &circle, const Vector3 &position)
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

double Zone::getValue(const Vector3 &location, const Vector3 &normal) const
{
    int i;
    double final_height;
    double value, value_height, value_steepness, value_circle;

    if (circles_included_count > 0)
    {
        value_circle = 0.0;
        for (i = 0; i < circles_included_count; i++)
        {
            value = _getCircleInfluence(circles_included[i], location);
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

    if (absolute_height)
    {
        final_height = location.y;
    }
    else
    {
        if (location.y >= relative_height_max)
        {
            final_height = 1.0;
        }
        else if (location.y <= relative_height_min)
        {
            final_height = 0.0;
        }
        else if (location.y <= relative_height_middle)
        {
            final_height = 0.5 * (location.y - relative_height_min) / (relative_height_middle - relative_height_min);
        }
        else
        {
            final_height = 0.5 + 0.5 * (location.y - relative_height_middle) / (relative_height_max - relative_height_middle);
        }
    }

    value_height = value_by_height->getValue(final_height);
    value_steepness = value_by_slope->getValue(1.0 - normal.y);

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

