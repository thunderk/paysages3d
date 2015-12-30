#ifndef MATHS_H
#define MATHS_H

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

    static constexpr double PI = 3.141592653589793238462643383279;
    static constexpr double PI_2 = PI / 2.0;
    static constexpr double PI_4 = PI / 4.0;
    static constexpr double TWOPI = 2.0 * PI;
};
}
}

#endif // MATHS_H
