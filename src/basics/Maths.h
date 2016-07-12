#pragma once

#include "basics_global.h"

namespace paysages {
namespace basics {

/**
 * Basic math utilities.
 */
class BASICSSHARED_EXPORT Maths {
  public:
    /**
     * Constraint a value in the [min,max[ range, by applying a modulo.
     */
    static double modInRange(double value, double min, double max);

    /**
     * Constraint a value in the [min,max[ range, by clamping it.
     */
    static double clamp(double x, double minval, double maxval);

    /**
     * Returns 0.0 when x < edge0, 1.0 when y > edge1, and a smoothly interpolated value in-between.
     */
    static double smoothstep(double edge0, double edge1, double x);

    /**
     * Find the length in a segment (with border values "edge0" and "edge1"), at which the value crosses 0.
     *
     * The length is not normalized (the middle of a segment of length 0.2 is 0.1).
     */
    static double zeroPoint(double segment_length, double edge0, double edge1);

    static constexpr double PI = 3.141592653589793238462643383279;
    static constexpr double PI_2 = PI / 2.0;
    static constexpr double PI_4 = PI / 4.0;
    static constexpr double TWOPI = 2.0 * PI;
};
}
}
