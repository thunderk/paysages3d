#ifndef VECTOR3_H
#define VECTOR3_H

#include "basics_global.h"

namespace paysages {
namespace basics {

/*
 * Cartesian coordinates (X, Y, Z) - right handed :
 *
 *        Y (up)
 *        |
 *        |
 *        +----X
 *       /
 *      /
 *     Z
 *
 * Spherical coordinates (R, PHI, THETA) :
 *
 *   R is the distance to origin
 *   PHI is typical azimuth
 *   THETA is elevation angle (not polar angle!)
 *
 *   X=1  Y=0  Z=0   =>  PHI=0
 *   X=0  Y=0  Z=-1  =>  PHI=PI/2
 *   X=-1 Y=0  Z=0   =>  PHI=PI
 *   X=0  Y=0  Z=1   =>  PHI=3*PI/2
 *
 *   X=1  Y=0  Z=0   =>  THETA=0
 *   X=0  Y=1  Z=0   =>  THETA=PI/2
 *   X=0  Y=-1 Z=0   =>  THETA=-PI/2
 */

typedef struct
{
    double r;
    double theta;
    double phi;
} VectorSpherical;

class BASICSSHARED_EXPORT Vector3
{
public:
    Vector3();
    Vector3(double x, double y, double z);
    Vector3(const VectorSpherical &v);

    void save(PackStream *stream) const;
    void load(PackStream *stream);

    Vector3 inverse() const;
    Vector3 scale(double scaling) const;
    double getNorm() const;
    Vector3 normalize() const;

    Vector3 add(double x, double y, double z) const;
    Vector3 add(const Vector3 &other) const;
    Vector3 sub(const Vector3 &other) const;
    double dotProduct(const Vector3 &other) const;
    Vector3 crossProduct(const Vector3 &other) const;

    VectorSpherical toSpherical() const;

public:
    // TODO Make private
    double x;
    double y;
    double z;
};

BASICSSHARED_EXPORT extern const Vector3 VECTOR_ZERO;
BASICSSHARED_EXPORT extern const Vector3 VECTOR_DOWN;
BASICSSHARED_EXPORT extern const Vector3 VECTOR_UP;
BASICSSHARED_EXPORT extern const Vector3 VECTOR_NORTH;
BASICSSHARED_EXPORT extern const Vector3 VECTOR_SOUTH;
BASICSSHARED_EXPORT extern const Vector3 VECTOR_EAST;
BASICSSHARED_EXPORT extern const Vector3 VECTOR_WEST;

}
}

// Inlining
#if PAYSAGES_USE_INLINING
#ifndef VECTOR3_INLINE_CPP
#include "Vector3.inline.cpp"
#endif
#endif

// Compat API

static inline Vector3 v3(double x, double y, double z)
{
    return Vector3(x, y, z);
}
static inline void v3Save(PackStream* stream, Vector3* v)
{
    v->save(stream);
}
static inline void v3Load(PackStream* stream, Vector3* v)
{
    v->load(stream);
}
static inline Vector3 v3Translate(const Vector3 &v1, double x, double y, double z)
{
    return v1.add(x, y, z);
}
static inline Vector3 v3Add(const Vector3 &v1, const Vector3 &v2)
{
    return v1.add(v2);
}
static inline Vector3 v3Sub(const Vector3 &v1, const Vector3 &v2)
{
    return v1.sub(v2);
}
static inline Vector3 v3Neg(const Vector3 &v)
{
    return v.inverse();
}
static inline Vector3 v3Scale(const Vector3 &v, double scale)
{
    return v.scale(scale);
}
static inline double v3Norm(const Vector3 &v)
{
    return v.getNorm();
}
static inline Vector3 v3Normalize(const Vector3 &v)
{
    return v.normalize();
}
static inline double v3Dot(const Vector3 &v1, const Vector3 &v2)
{
    return v1.dotProduct(v2);
}
static inline Vector3 v3Cross(const Vector3 &v1, const Vector3 &v2)
{
    return v1.crossProduct(v2);
}
static inline VectorSpherical v3ToSpherical(const Vector3 &v)
{
    return v.toSpherical();
}
static inline Vector3 v3FromSpherical(const VectorSpherical &v)
{
    return Vector3(v);
}

#endif // VECTOR3_H
