#ifndef CURVE_H
#define CURVE_H

#include "basics_global.h"

namespace paysages {
namespace basics {

typedef struct {
    double position;
    double value;
} CurvePoint;

class BASICSSHARED_EXPORT Curve {
  public:
    Curve();
    ~Curve();

    void copy(Curve *destination) const;
    void save(PackStream *stream) const;
    void load(PackStream *stream);
    void validate();

    inline int getPointCount() const {
        return nbpoints;
    }
    CurvePoint getPoint(int number) const;
    bool getPoint(int number, CurvePoint *point) const;

    double getValue(double position) const;

    void clear();
    void setDefault(double value);
    int addPoint(const CurvePoint &point);
    int addPoint(double position, double value);
    void setPoint(int number, const CurvePoint &point);
    void removePoint(int number);

  private:
    double default_value;
    int nbpoints;
    CurvePoint *points;
};
}
}

#endif // CURVE_H
