#include "Interpolation.h"

double Interpolation::bicubic(double stencil[16], double x, double y) {
    double buf_cubic_y[4];

    buf_cubic_y[0] = Interpolation::cubic(stencil, x);
    buf_cubic_y[1] = Interpolation::cubic(stencil + 4, x);
    buf_cubic_y[2] = Interpolation::cubic(stencil + 8, x);
    buf_cubic_y[3] = Interpolation::cubic(stencil + 12, x);

    return Interpolation::cubic(buf_cubic_y, y);
}

double Interpolation::bilinear(double p[4], double x, double y) {
    double e1 = linear(p[0], p[1], x);
    double e2 = linear(p[3], p[2], x);
    return Interpolation::linear(e1, e2, y);
}

double Interpolation::trilinear(double p[8], double x, double y, double z) {
    double f1 = bilinear(p, x, y);
    double f2 = bilinear(p + 4, x, y);
    return Interpolation::linear(f1, f2, z);
}
