#ifndef MATRIX4_H
#define MATRIX4_H

#include "basics_global.h"

namespace paysages {
namespace basics {

class BASICSSHARED_EXPORT Matrix4 {
  public:
    Matrix4(bool identity = true);
    Matrix4(double a, double b, double c, double d, double e, double f, double g, double h, double i, double j,
            double k, double l, double m, double n, double o, double p);

    void save(PackStream *stream) const;
    void load(PackStream *stream);

    Matrix4 mult(const Matrix4 &other) const;
    Vector3 multPoint(const Vector3 &v) const;
    Vector3 transform(const Vector3 &v) const;
    Matrix4 transposed() const;
    Matrix4 inversed() const;

    static Matrix4 newScale(double x, double y, double z);
    static Matrix4 newTranslate(double x, double y, double z);
    static Matrix4 newRotateX(double angle);
    static Matrix4 newRotateY(double angle);
    static Matrix4 newRotateZ(double angle);
    static Matrix4 newRotateAxis(double angle, const Vector3 &axis);
    static Matrix4 newRotateEuler(double heading, double attitude, double bank);
    static Matrix4 newRotateTripleAxis(const Vector3 &x, const Vector3 &y, const Vector3 &z);
    static Matrix4 newLookAt(const Vector3 &eye, const Vector3 &at, const Vector3 &up);
    static Matrix4 newPerspective(double fov_y, double aspect, double near, double far);

    double getDeterminant() const;

    inline double getA() const {
        return a;
    }
    inline double getB() const {
        return b;
    }
    inline double getC() const {
        return c;
    }
    inline double getD() const {
        return d;
    }
    inline double getE() const {
        return e;
    }
    inline double getF() const {
        return f;
    }
    inline double getG() const {
        return g;
    }
    inline double getH() const {
        return h;
    }
    inline double getI() const {
        return i;
    }
    inline double getJ() const {
        return j;
    }
    inline double getK() const {
        return k;
    }
    inline double getL() const {
        return l;
    }
    inline double getM() const {
        return m;
    }
    inline double getN() const {
        return n;
    }
    inline double getO() const {
        return o;
    }
    inline double getP() const {
        return p;
    }

  private:
    double a;
    double b;
    double c;
    double d;
    double e;
    double f;
    double g;
    double h;
    double i;
    double j;
    double k;
    double l;
    double m;
    double n;
    double o;
    double p;
};
}
}

#endif // MATRIX4_H
