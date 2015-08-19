#include "ColorHSL.h"

#include "Color.h"

static inline double _hue2rgb(double p, double q, double t)
{
    if (t < 0.0) t += 1;
    if (t > 1.0) t -= 1;
    if (t < 1.0 / 6.0) return p + (q - p) * 6.0 * t;
    if (t < 1.0 / 2.0) return q;
    if (t < 2.0 / 3.0) return p + (q - p) * (2.0 / 3.0 - t) * 6.0;
    return p;
}

Color colorFromHSL(const ColorHSL &col)
{
    Color result;

    if (col.s == 0)
    {
        result.r = result.g = result.b = col.l;
    }
    else
    {
        double q = col.l < 0.5 ? col.l * (1.0 + col.s) : col.l + col.s - col.l * col.s;
        double p = 2 * col.l - q;
        result.r = _hue2rgb(p, q, col.h + 1.0 / 3.0);
        result.g = _hue2rgb(p, q, col.h);
        result.b = _hue2rgb(p, q, col.h - 1.0 / 3.0);
    }

    result.a = col.a;

    return result;
}

ColorHSL colorToHSL(const Color &col)
{
    ColorHSL result;
    double min, max;

    max = col.r > col.g ? col.r : col.g;
    max = col.b > max ? col.b : max;

    min = col.r < col.g ? col.r : col.g;
    min = col.b < min ? col.b : min;

    result.h = result.s = result.l = (max + min) / 2.0;

    if (max == min)
    {
        result.h = result.s = 0.0;
    }
    else
    {
        double d = max - min;
        result.s = result.l > 0.5 ? d / (2.0 - max - min) : d / (max + min);
        if (max == col.r)
        {
            result.h = (col.g - col.b) / d + (col.g < col.b ? 6.0 : 0);
        }
        else if (max == col.g)
        {
            result.h = (col.b - col.r) / d + 2.0;
        }
        else
        {
            result.h = (col.r - col.g) / d + 4.0;
        }
        result.h /= 6.0;
    }

    result.a = col.a;

    return result;
}

ColorHSL colorHSLFromValues(double h, double s, double l, double a)
{
    ColorHSL result;

    result.h = h;
    result.s = s;
    result.l = l;
    result.a = a;

    return result;
}
