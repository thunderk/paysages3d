#ifndef VECTOR3_H
#define VECTOR3_H

#include "basics_global.h"

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

namespace paysages {
namespace system {class PackStream;}
}

typedef struct
{
    double x;
    double y;
    double z;
} Vector3;

typedef struct
{
    double r;
    double theta;
    double phi;
} VectorSpherical;

BASICSSHARED_EXPORT extern Vector3 VECTOR_ZERO;
BASICSSHARED_EXPORT extern Vector3 VECTOR_DOWN;
BASICSSHARED_EXPORT extern Vector3 VECTOR_UP;
BASICSSHARED_EXPORT extern Vector3 VECTOR_NORTH;
BASICSSHARED_EXPORT extern Vector3 VECTOR_SOUTH;
BASICSSHARED_EXPORT extern Vector3 VECTOR_EAST;
BASICSSHARED_EXPORT extern Vector3 VECTOR_WEST;

static inline Vector3 v3(double x, double y, double z)
{
    Vector3 result = {x, y, z};
    return result;
}
BASICSSHARED_EXPORT void v3Save(PackStream* stream, Vector3* v);
BASICSSHARED_EXPORT void v3Load(PackStream* stream, Vector3* v);
BASICSSHARED_EXPORT Vector3 v3Translate(Vector3 v1, double x, double y, double z);
BASICSSHARED_EXPORT Vector3 v3Add(Vector3 v1, Vector3 v2);
BASICSSHARED_EXPORT Vector3 v3Sub(Vector3 v1, Vector3 v2);
BASICSSHARED_EXPORT Vector3 v3Neg(Vector3 v);
BASICSSHARED_EXPORT Vector3 v3Scale(Vector3 v, double scale);
BASICSSHARED_EXPORT double v3Norm(Vector3 v);
BASICSSHARED_EXPORT Vector3 v3Normalize(Vector3 v);
BASICSSHARED_EXPORT double v3Dot(Vector3 v1, Vector3 v2);
BASICSSHARED_EXPORT Vector3 v3Cross(Vector3 v1, Vector3 v2);
BASICSSHARED_EXPORT VectorSpherical v3ToSpherical(Vector3 v);
BASICSSHARED_EXPORT Vector3 v3FromSpherical(VectorSpherical v);

#endif // VECTOR3_H
