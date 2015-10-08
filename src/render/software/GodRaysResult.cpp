#include "GodRaysResult.h"

GodRaysResult::GodRaysResult(double inside_length, double full_length):
    inside_length(inside_length), full_length(full_length)
{
}

Color GodRaysResult::apply(const Color &raw, const Color &atmosphered, const GodRaysParams &params)
{
    if (inside_length == 0.0)
    {
        return raw;
    }
    else if (inside_length < full_length)
    {
        double diff = full_length - inside_length;
        double factor = 1.0 - params.penetration * diff;
        double minimum = params.resistance;
        double complement = 1.0 - minimum;
        if (factor < minimum)
        {
            factor = minimum;
        }
        else
        {
            factor = pow((factor - minimum) / complement, params.boost) * complement + minimum;
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
