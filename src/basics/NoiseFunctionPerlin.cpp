#include "NoiseFunctionPerlin.h"

/*
 * Perlin noise implementation.
 *
 * Based on Ken Perlin implementation.
 */

#include <cstdlib>
#include <cmath>

NoiseFunctionPerlin::NoiseFunctionPerlin() {
}

#define B 0x100
#define BM 0xff

#define N 0x1000
#define NP 12 /* 2^N */
#define NM 0xfff

static int p[B + B + 2];
static double g3[B + B + 2][3];
static double g2[B + B + 2][2];
static double g1[B + B + 2];

#define s_curve(t) (t * t * (3. - 2. * t))

#define lerp(t, a, b) (a + t * (b - a))

#define setup(i, b0, b1, r0, r1)                                                                                       \
    t = vec[i] + N;                                                                                                    \
    b0 = trunc_to_int(t) & BM;                                                                                         \
    b1 = (b0 + 1) & BM;                                                                                                \
    r0 = t - trunc_to_int(t);                                                                                          \
    r1 = r0 - 1.;

double noisePerlinGet1DValue(double x) {
    double vec[1] = {x * 2.0};
    int bx0, bx1;
    double rx0, rx1, sx, t, u, v;

    setup(0, bx0, bx1, rx0, rx1);

    sx = s_curve(rx0);

    u = rx0 * g1[p[bx0]];
    v = rx1 * g1[p[bx1]];

    return lerp(sx, u, v) * 1.068 + 0.5;
}

double noisePerlinGet2DValue(double x, double y) {
    double vec[2] = {x * 2.0, y * 2.0};
    int bx0, bx1, by0, by1, b00, b10, b01, b11;
    double rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
    int i, j;

    setup(0, bx0, bx1, rx0, rx1);
    setup(1, by0, by1, ry0, ry1);

    i = p[bx0];
    j = p[bx1];

    b00 = p[i + by0];
    b10 = p[j + by0];
    b01 = p[i + by1];
    b11 = p[j + by1];

    sx = s_curve(rx0);
    sy = s_curve(ry0);

#define at2(rx, ry) (rx * q[0] + ry * q[1])

    q = g2[b00];
    u = at2(rx0, ry0);
    q = g2[b10];
    v = at2(rx1, ry0);
    a = lerp(sx, u, v);

    q = g2[b01];
    u = at2(rx0, ry1);
    q = g2[b11];
    v = at2(rx1, ry1);
    b = lerp(sx, u, v);

    return lerp(sy, a, b) * 0.709 + 0.5;
}

double noisePerlinGet3DValue(double x, double y, double z) {
    double vec[3] = {x * 2.0, y * 2.0, z * 2.0};
    int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
    double rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
    int i, j;

    setup(0, bx0, bx1, rx0, rx1);
    setup(1, by0, by1, ry0, ry1);
    setup(2, bz0, bz1, rz0, rz1);

    i = p[bx0];
    j = p[bx1];

    b00 = p[i + by0];
    b10 = p[j + by0];
    b01 = p[i + by1];
    b11 = p[j + by1];

    t = s_curve(rx0);
    sy = s_curve(ry0);
    sz = s_curve(rz0);

#define at3(rx, ry, rz) (rx * q[0] + ry * q[1] + rz * q[2])

    q = g3[b00 + bz0];
    u = at3(rx0, ry0, rz0);
    q = g3[b10 + bz0];
    v = at3(rx1, ry0, rz0);
    a = lerp(t, u, v);

    q = g3[b01 + bz0];
    u = at3(rx0, ry1, rz0);
    q = g3[b11 + bz0];
    v = at3(rx1, ry1, rz0);
    b = lerp(t, u, v);

    c = lerp(sy, a, b);

    q = g3[b00 + bz1];
    u = at3(rx0, ry0, rz1);
    q = g3[b10 + bz1];
    v = at3(rx1, ry0, rz1);
    a = lerp(t, u, v);

    q = g3[b01 + bz1];
    u = at3(rx0, ry1, rz1);
    q = g3[b11 + bz1];
    v = at3(rx1, ry1, rz1);
    b = lerp(t, u, v);

    d = lerp(sy, a, b);

    return lerp(sz, c, d) * 0.661 + 0.5;
}

static void _normalize2(double v[2]) {
    double s;

    s = sqrt(v[0] * v[0] + v[1] * v[1]);
    v[0] = v[0] / s;
    v[1] = v[1] / s;
}

static void _normalize3(double v[3]) {
    double s;

    s = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    v[0] = v[0] / s;
    v[1] = v[1] / s;
    v[2] = v[2] / s;
}

static int noisePerlinInit(void) {
    int i, j, k;

    for (i = 0; i < B; i++) {
        p[i] = i;

        g1[i] = to_double((rand() % (B + B)) - B) / B;

        for (j = 0; j < 2; j++)
            g2[i][j] = to_double((rand() % (B + B)) - B) / B;
        _normalize2(g2[i]);

        for (j = 0; j < 3; j++)
            g3[i][j] = to_double((rand() % (B + B)) - B) / B;
        _normalize3(g3[i]);
    }

    while (--i) {
        k = p[i];
        p[i] = p[j = rand() % B];
        p[j] = k;
    }

    for (i = 0; i < B + 2; i++) {
        p[B + i] = p[i];
        g1[B + i] = g1[i];
        for (j = 0; j < 2; j++)
            g2[B + i][j] = g2[i][j];
        for (j = 0; j < 3; j++)
            g3[B + i][j] = g3[i][j];
    }

    return 1;
}

static int _inited = noisePerlinInit();
