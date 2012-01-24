#include "shared/types.h"
#include "shared/functions.h"
#include "shared/globals.h"
#include "shared/constants.h"

static double _near = 0.0, _far = 1.0;
static Color _col = {0.0, 0.0, 0.0, 0.0};

void fogSave(FILE* f)
{
}

void fogLoad(FILE* f)
{
}

void fogSetColor(Color col)
{
    _col = col;
}

void fogSetDistance(double near, double far)
{
    _near = near;
    _far = far;
}

Color fogApplyToLocation(Vector3 location, Color base)
{
    return base;
    // TODO Don't use camera_location
    /*Color mask = _col;
    double distance = v3Norm(v3Sub(camera_location, location));
    double value;

    if (distance < _near)
    {
        return base;
    }
    else if (distance > _far)
    {
        distance = _far;
    }

    value = 0.8 * (distance - _near) / (_far - _near);

    mask.a *= value;
    colorMask(&base, &mask);
    return base;*/
}

