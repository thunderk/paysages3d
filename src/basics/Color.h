#pragma once

#include "basics_global.h"

namespace paysages {
namespace basics {

class BASICSSHARED_EXPORT Color {
  public:
    Color() = default;
    Color(const Color &col);
    Color(double r, double g, double b, double a = 1.0);

    void save(PackStream *stream) const;
    void load(PackStream *stream);

    unsigned int to32BitRGBA() const;
    unsigned int to32BitBGRA() const;
    unsigned int to32BitARGB() const;
    unsigned int to32BitABGR() const;

    static Color from32BitRGBA(unsigned int col);
    static Color from32BitBGRA(unsigned int col);
    static Color from32BitARGB(unsigned int col);
    static Color from32BitABGR(unsigned int col);

    void mask(const Color &mask);
    double normalize();
    Color normalized();
    double getValue() const;
    double getPower() const;
    void limitPower(double max_power);

    /**
     * Scale the RGB components by a factor.
     */
    void scale(double factor);
    /**
     * Return a copy, with RGB components scaled by a factor.
     */
    Color scaled(double factor) const;

    Color add(const Color &other) const;
    Color lerp(const Color &other, double f) const;

    inline bool operator==(const Color &other) const {
        return r == other.r and g == other.g and b == other.b and a == other.a;
    }

  public:
    // TODO Make private
    double r;
    double g;
    double b;
    double a;
};

BASICSSHARED_EXPORT extern const Color COLOR_TRANSPARENT;
BASICSSHARED_EXPORT extern const Color COLOR_BLACK;
BASICSSHARED_EXPORT extern const Color COLOR_RED;
BASICSSHARED_EXPORT extern const Color COLOR_GREEN;
BASICSSHARED_EXPORT extern const Color COLOR_BLUE;
BASICSSHARED_EXPORT extern const Color COLOR_WHITE;
BASICSSHARED_EXPORT extern const Color COLOR_GREY;
}
}
