#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include "basics_global.h"

namespace paysages {
namespace basics {

class BASICSSHARED_EXPORT Interpolation {
  public:
    static inline double linear(double p1, double p2, double x) {
        return p1 + x * (p2 - p1);
    }
    static double bilinear(double p[4], double x, double y);
    static double trilinear(double p[8], double x, double y, double z);

    static inline double cubic(double p[4], double x) {
        return p[1] +
               0.5 * x * (p[2] - p[0] +
                          x * (2.0 * p[0] - 5.0 * p[1] + 4.0 * p[2] - p[3] + x * (3.0 * (p[1] - p[2]) + p[3] - p[0])));
    }
    static double bicubic(double stencil[16], double x, double y);
};
}
}

#endif // INTERPOLATION_H
