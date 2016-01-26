#include "Maths.h"

#include <cmath>
#include <algorithm>

double Maths::modInRange(double value, double min, double max) {
    double size = max - min;
    if (size < 0.0000000000001) {
        return value;
    } else {
        double norm = (value - min) / size;
        double modulo = norm - floor(norm);
        return modulo * size + min;
    }
}

double Maths::clamp(double x, double minval, double maxval) {
    return min(max(x, minval), maxval);
}

double Maths::smoothstep(double edge0, double edge1, double x) {
    x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    return x * x * (3.0 - 2.0 * x);
}
