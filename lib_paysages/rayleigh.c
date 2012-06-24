#include "rayleigh.h"

#include <math.h>

/*static double _phase(double g, double theta)
{
    double g2 = g * g;
    double costheta = cos(theta);
    return ((3.0 * (1.0 - g2)) / (2.0 * (2.0 + g2))) * ((1.0 + costheta * costheta) / exp(1.0 + g2 - 2.0 * g * costheta, 1.5));
}

static double _intOpticalDepthCallback(double h, double* h0)
{
    return -h / *h0;
}

static double _opticalDepth(double h0, double lambda, double k, Vector3 start, Vector3 end)
{
    return 4.0 * M_PI * k * toolsIntegrate(_intOpticalDepthCallback);
}*/

Color rayleighGetSkyColor(Vector3 viewer, Vector3 direction, Vector3 sun_direction)
{
    return COLOR_BLACK;
}

Color rayleighApplyToObject(Vector3 viewer, Vector3 object_location, Vector3 sun_direction, Color object_color)
{
    return COLOR_BLACK;
}
