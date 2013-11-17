#include "Interpolation.h"

double Interpolation::bicubic(double stencil[16], double x, double y)
{
    double buf_cubic_y[4];

    buf_cubic_y[0] = Interpolation::cubic(stencil, x);
    buf_cubic_y[1] = Interpolation::cubic(stencil + 4, x);
    buf_cubic_y[2] = Interpolation::cubic(stencil + 8, x);
    buf_cubic_y[3] = Interpolation::cubic(stencil + 12, x);

    return Interpolation::cubic(buf_cubic_y, y);
}
