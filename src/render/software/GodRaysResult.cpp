#include "GodRaysResult.h"

GodRaysResult::GodRaysResult(double inside_length, double full_length):
    inside_length(inside_length), full_length(full_length)
{
}

Color GodRaysResult::apply(const Color &raw, const Color &atmosphered)
{
    if (inside_length == 0.0)
    {
        return raw;
    }
    else if (inside_length < full_length)
    {
        double diff = full_length - inside_length;
        double factor = 1.0 - 0.01 * diff;
        if (factor < 0.3)
        {
            factor = 0.3;
        }
        else
        {
            factor = pow((factor - 0.3) / 0.7, 8.0) * 0.7 + 0.3;
        }

        return Color(raw.r + (atmosphered.r - raw.r) * factor,
                     raw.g + (atmosphered.g - raw.g) * factor,
                     raw.b + (atmosphered.b - raw.b) * factor,
                     atmosphered.a);
    }
    else
    {
        return atmosphered;
    }
}
