#ifndef COLOR_H
#define COLOR_H

#include "basics_global.h"

namespace paysages {
namespace basics {

class Color
{
public:
    Color();
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

// Compat API

static inline void colorSave(PackStream* stream, Color* col)
{
    col->save(stream);
}
static inline void colorLoad(PackStream* stream, Color* col)
{
    col->load(stream);
}
static inline Color colorFromValues(double r, double g, double b, double a)
{
    return Color(r, g, b, a);
}
static inline unsigned int colorTo32BitRGBA(Color* col)
{
    return col->to32BitRGBA();
}
static inline unsigned int colorTo32BitBGRA(Color* col)
{
    return col->to32BitBGRA();
}
static inline unsigned int colorTo32BitARGB(Color* col)
{
    return col->to32BitARGB();
}
static inline unsigned int colorTo32BitABGR(Color* col)
{
    return col->to32BitABGR();
}
static inline Color colorFrom32BitRGBA(unsigned int col)
{
    return Color::from32BitRGBA(col);
}
static inline Color colorFrom32BitBGRA(unsigned int col)
{
    return Color::from32BitBGRA(col);
}
static inline Color colorFrom32BitARGB(unsigned int col)
{
    return Color::from32BitARGB(col);
}
static inline Color colorFrom32BitABGR(unsigned int col)
{
    return Color::from32BitABGR(col);
}
static inline void colorMask(Color* base, Color* mask)
{
    base->mask(*mask);
}
static inline double colorNormalize(Color* col)
{
    return col->normalize();
}
static inline double colorGetValue(Color* col)
{
    return col->getValue();
}
static inline double colorGetPower(Color* col)
{
    return col->getPower();
}
static inline void colorLimitPower(Color* col, double max_power)
{
    col->limitPower(max_power);
}

#endif // COLOR_H
