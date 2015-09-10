#ifndef COLOR_H
#define COLOR_H

#include "basics_global.h"

namespace paysages {
namespace basics {

class BASICSSHARED_EXPORT Color
{
public:
    Color() = default;
    Color(const Color &col);
    Color(double r, double g, double b, double a=1.0);

    void save(PackStream* stream) const;
    void load(PackStream* stream);

    unsigned int to32BitRGBA() const;
    unsigned int to32BitBGRA() const;
    unsigned int to32BitARGB() const;
    unsigned int to32BitABGR() const;

    static Color from32BitRGBA(unsigned int col);
    static Color from32BitBGRA(unsigned int col);
    static Color from32BitARGB(unsigned int col);
    static Color from32BitABGR(unsigned int col);

    void mask(const Color& mask);
    double normalize();
    double getValue() const;
    double getPower() const;
    void limitPower(double max_power);

    Color add(const Color& other) const;
    Color lerp(const Color& other, double f) const;

public:
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

// Inlining
#if PAYSAGES_USE_INLINING
#ifndef COLOR_INLINE_CPP
#include "Color.inline.cpp"
#endif
#endif

#endif // COLOR_H
