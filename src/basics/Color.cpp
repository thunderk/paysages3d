#include "Color.h"

#include "PackStream.h"
#include <cassert>
#include <cmath>

const Color paysages::basics::COLOR_TRANSPARENT = {0.0, 0.0, 0.0, 0.0};
const Color paysages::basics::COLOR_BLACK = {0.0, 0.0, 0.0, 1.0};
const Color paysages::basics::COLOR_RED = {1.0, 0.0, 0.0, 1.0};
const Color paysages::basics::COLOR_GREEN = {0.0, 1.0, 0.0, 1.0};
const Color paysages::basics::COLOR_BLUE = {0.0, 0.0, 1.0, 1.0};
const Color paysages::basics::COLOR_WHITE = {1.0, 1.0, 1.0, 1.0};
const Color paysages::basics::COLOR_GREY = {0.5, 0.5, 0.5, 1.0};

Color::Color(double r, double g, double b, double a) : r(r), g(g), b(b), a(a) {
}

Color::Color(const Color &col) : r(col.r), g(col.g), b(col.b), a(col.a) {
}

void Color::save(PackStream *stream) const {
    stream->write(&r);
    stream->write(&g);
    stream->write(&b);
    stream->write(&a);
}

void Color::load(PackStream *stream) {
    stream->read(&r);
    stream->read(&g);
    stream->read(&b);
    stream->read(&a);
}

unsigned int Color::to32BitRGBA() const {
    return (((unsigned int)(a * 255.0)) << 24) | (((unsigned int)(b * 255.0)) << 16) |
           (((unsigned int)(g * 255.0)) << 8) | ((unsigned int)(r * 255.0));
}

unsigned int Color::to32BitBGRA() const {
    return (((unsigned int)(a * 255.0)) << 24) | (((unsigned int)(r * 255.0)) << 16) |
           (((unsigned int)(g * 255.0)) << 8) | ((unsigned int)(b * 255.0));
}

unsigned int Color::to32BitARGB() const {
    return (((unsigned int)(b * 255.0)) << 24) | (((unsigned int)(g * 255.0)) << 16) |
           (((unsigned int)(r * 255.0)) << 8) | ((unsigned int)(a * 255.0));
}

unsigned int Color::to32BitABGR() const {
    return (((unsigned int)(r * 255.0)) << 24) | (((unsigned int)(g * 255.0)) << 16) |
           (((unsigned int)(b * 255.0)) << 8) | ((unsigned int)(a * 255.0));
}

Color Color::from32BitRGBA(unsigned int col) {
    return Color((to_double(col & 0x000000FF)) / 255.0, (to_double((col & 0x0000FF00) >> 8)) / 255.0,
                 (to_double((col & 0x00FF0000) >> 16)) / 255.0, (to_double((col & 0xFF000000) >> 24)) / 255.0);
}

Color Color::from32BitBGRA(unsigned int col) {
    return Color((to_double(col & 0x000000FF)) / 255.0, (to_double((col & 0x0000FF00) >> 8)) / 255.0,
                 (to_double((col & 0x00FF0000) >> 16)) / 255.0, (to_double((col & 0xFF000000) >> 24)) / 255.0);
}

Color Color::from32BitARGB(unsigned int col) {
    return Color((to_double(col & 0x000000FF)) / 255.0, (to_double((col & 0x0000FF00) >> 8)) / 255.0,
                 (to_double((col & 0x00FF0000) >> 16)) / 255.0, (to_double((col & 0xFF000000) >> 24)) / 255.0);
}

Color Color::from32BitABGR(unsigned int col) {
    return Color((to_double(col & 0x000000FF)) / 255.0, (to_double((col & 0x0000FF00) >> 8)) / 255.0,
                 (to_double((col & 0x00FF0000) >> 16)) / 255.0, (to_double((col & 0xFF000000) >> 24)) / 255.0);
}

void Color::mask(const Color &mask) {
    double new_a;
    new_a = a + mask.a - (a * mask.a);
    r = (mask.r * mask.a + r * a - r * a * mask.a) / new_a;
    g = (mask.g * mask.a + g * a - g * a * mask.a) / new_a;
    b = (mask.b * mask.a + b * a - b * a * mask.a) / new_a;
    a = new_a;

    /*double mask_weight = mask->a;
    double base_weight = 1.0 - mask_weight;

    base->r = mask->r * mask_weight + base->r * base_weight;
    base->g = mask->g * mask_weight + base->g * base_weight;
    base->b = mask->b * mask_weight + base->b * base_weight;
    base->a = base->a + mask_weight * (1.0 - base->a);*/
}

double Color::normalize() {
#ifndef NDEBUG
    assert(r >= 0.0);
    assert(g >= 0.0);
    assert(b >= 0.0);
    assert(a >= 0.0);
#ifdef isnan
    assert(!isnan(r));
    assert(!isnan(g));
    assert(!isnan(b));
    assert(!isnan(a));
#endif
#ifdef isfinite
    assert(isfinite(r));
    assert(isfinite(g));
    assert(isfinite(b));
    assert(isfinite(a));
#endif
#endif

    if (r > 1.0) {
        r = 1.0;
    }
    if (g > 1.0) {
        g = 1.0;
    }
    if (b > 1.0) {
        b = 1.0;
    }
    return 1.0;
    /*double max = colorGetValue(col);

    assert(max >= 0.0);

    if (max > 1.0)
    {
        col->r /= max;
        col->g /= max;
        col->b /= max;
    }
    return max;*/
}

Color Color::normalized() {
    Color col = *this;
    col.normalize();
    return col;
}

double Color::getValue() const {
    double max;

    max = r;
    if (g > max) {
        max = g;
    }
    if (b > max) {
        max = b;
    }
    return max;
}

double Color::getPower() const {
    return r + g + b;
}

void Color::limitPower(double max_power) {
    double power = r + g + b;

    if (power > max_power) {
        double factor = max_power / power;

        r *= factor;
        g *= factor;
        b *= factor;
    }
}

void Color::scale(double factor) {
    r *= factor;
    g *= factor;
    b *= factor;
}

Color Color::scaled(double factor) const {
    return Color(r * factor, g * factor, b * factor, a);
}

Color Color::add(const Color &other) const {
    return Color(r + other.r, g + other.g, b + other.b, a);
}

Color Color::lerp(const Color &other, double f) const {
    Color result(r * (1.0 - f) + other.r * f, g * (1.0 - f) + other.g * f, b * (1.0 - f) + other.b * f,
                 a * (1.0 - f) + other.a * f);
    return result;
}
