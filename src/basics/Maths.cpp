#include "Maths.h"

#include <cmath>

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
