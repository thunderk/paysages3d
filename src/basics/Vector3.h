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
    Vector3() = default;
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

    /**
     * Get the mid-point of the segment between *this* point and *other*.
     */
    Vector3 midPointTo(const Vector3 &other) const;

    VectorSpherical toSpherical() const;

    /**
     * Produce a random vector in a sphere domain.
     *
     * If *only_surface* is true, produce a vector with *radius* as length.
     */
    static Vector3 randomInSphere(double radius=1.0, bool only_surface=false);

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

#endif // VECTOR3_H
