#ifndef _PAYSAGES_TOOLS_EUCLID_H_
#define _PAYSAGES_TOOLS_EUCLID_H_

#include "../rendering_global.h"

// TEMP
#include "Vector3.h"

namespace paysages {
namespace system {class PackStream;}
}

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
