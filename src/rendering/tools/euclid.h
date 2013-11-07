#ifndef _PAYSAGES_TOOLS_EUCLID_H_
#define _PAYSAGES_TOOLS_EUCLID_H_

#include "../rendering_global.h"

namespace paysages {
namespace system {class PackStream;}
}

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

typedef struct
{
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
} Matrix4;

RENDERINGSHARED_EXPORT extern Vector3 VECTOR_ZERO;
RENDERINGSHARED_EXPORT extern Vector3 VECTOR_DOWN;
RENDERINGSHARED_EXPORT extern Vector3 VECTOR_UP;
RENDERINGSHARED_EXPORT extern Vector3 VECTOR_NORTH;
RENDERINGSHARED_EXPORT extern Vector3 VECTOR_SOUTH;
RENDERINGSHARED_EXPORT extern Vector3 VECTOR_EAST;
RENDERINGSHARED_EXPORT extern Vector3 VECTOR_WEST;

static inline Vector3 v3(double x, double y, double z)
{
    Vector3 result = {x, y, z};
    return result;
};
RENDERINGSHARED_EXPORT void v3Save(PackStream* stream, Vector3* v);
RENDERINGSHARED_EXPORT void v3Load(PackStream* stream, Vector3* v);
RENDERINGSHARED_EXPORT Vector3 v3Translate(Vector3 v1, double x, double y, double z);
RENDERINGSHARED_EXPORT Vector3 v3Add(Vector3 v1, Vector3 v2);
RENDERINGSHARED_EXPORT Vector3 v3Sub(Vector3 v1, Vector3 v2);
RENDERINGSHARED_EXPORT Vector3 v3Neg(Vector3 v);
RENDERINGSHARED_EXPORT Vector3 v3Scale(Vector3 v, double scale);
RENDERINGSHARED_EXPORT double v3Norm(Vector3 v);
RENDERINGSHARED_EXPORT Vector3 v3Normalize(Vector3 v);
RENDERINGSHARED_EXPORT double v3Dot(Vector3 v1, Vector3 v2);
RENDERINGSHARED_EXPORT Vector3 v3Cross(Vector3 v1, Vector3 v2);
RENDERINGSHARED_EXPORT VectorSpherical v3ToSpherical(Vector3 v);
RENDERINGSHARED_EXPORT Vector3 v3FromSpherical(VectorSpherical v);

RENDERINGSHARED_EXPORT void m4Save(PackStream* stream, Matrix4* m);
RENDERINGSHARED_EXPORT void m4Load(PackStream* stream, Matrix4* m);
RENDERINGSHARED_EXPORT Matrix4 m4NewIdentity();
RENDERINGSHARED_EXPORT Matrix4 m4Mult(Matrix4 m1, Matrix4 m2);
RENDERINGSHARED_EXPORT Vector3 m4MultPoint(Matrix4 m, Vector3 v);
RENDERINGSHARED_EXPORT Vector3 m4Transform(Matrix4 m, Vector3 v);
RENDERINGSHARED_EXPORT Matrix4 m4Transpose(Matrix4 m);
RENDERINGSHARED_EXPORT Matrix4 m4NewScale(double x, double y, double z);
RENDERINGSHARED_EXPORT Matrix4 m4NewTranslate(double x, double y, double z);
RENDERINGSHARED_EXPORT Matrix4 m4NewRotateX(double angle);
RENDERINGSHARED_EXPORT Matrix4 m4NewRotateY(double angle);
RENDERINGSHARED_EXPORT Matrix4 m4NewRotateZ(double angle);
RENDERINGSHARED_EXPORT Matrix4 m4NewRotateAxis(double angle, Vector3 axis);
RENDERINGSHARED_EXPORT Matrix4 m4NewRotateEuler(double heading, double attitude, double bank);
RENDERINGSHARED_EXPORT Matrix4 m4NewRotateTripleAxis(Vector3 x, Vector3 y, Vector3 z);
RENDERINGSHARED_EXPORT Matrix4 m4NewLookAt(Vector3 eye, Vector3 at, Vector3 up);
RENDERINGSHARED_EXPORT Matrix4 m4NewPerspective(double fov_y, double aspect, double near, double far);
RENDERINGSHARED_EXPORT double m4Determinant(Matrix4 m);
RENDERINGSHARED_EXPORT Matrix4 m4Inverse(Matrix4 m);

RENDERINGSHARED_EXPORT double euclidGet2DAngle(double x, double y);
RENDERINGSHARED_EXPORT Vector3 euclidGetNormalFromTriangle(Vector3 center, Vector3 bottom, Vector3 right);
RENDERINGSHARED_EXPORT double euclidGetDistance2D(double x1, double y1, double x2, double y2);
RENDERINGSHARED_EXPORT int euclidRayIntersectSphere(Vector3 ray_point, Vector3 ray_direction, Vector3 sphere_center, double sphere_radius, Vector3* hit1, Vector3* hit2);

#endif
