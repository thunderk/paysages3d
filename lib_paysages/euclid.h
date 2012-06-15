#ifndef _PAYSAGES_EUCLID_H_
#define _PAYSAGES_EUCLID_H_

#include "pack.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    float x;
    float y;
    float z;
} Vector3;

typedef struct
{
    float a;
    float b;
    float c;
    float d;
    float e;
    float f;
    float g;
    float h;
    float i;
    float j;
    float k;
    float l;
    float m;
    float n;
    float o;
    float p;
} Matrix4;

extern Vector3 VECTOR_ZERO;

void v3Save(PackStream* stream, Vector3* v);
void v3Load(PackStream* stream, Vector3* v);
Vector3 v3Translate(Vector3 v1, float x, float y, float z);
Vector3 v3Add(Vector3 v1, Vector3 v2);
Vector3 v3Sub(Vector3 v1, Vector3 v2);
Vector3 v3Neg(Vector3 v);
Vector3 v3Scale(Vector3 v, float scale);
float v3Norm(Vector3 v);
Vector3 v3Normalize(Vector3 v);
float v3Dot(Vector3 v1, Vector3 v2);
Vector3 v3Cross(Vector3 v1, Vector3 v2);

void m4Save(PackStream* stream, Matrix4* m);
void m4Load(PackStream* stream, Matrix4* m);
Matrix4 m4NewIdentity();
Matrix4 m4Mult(Matrix4 m1, Matrix4 m2);
Vector3 m4MultPoint(Matrix4 m, Vector3 v);
Vector3 m4Transform(Matrix4 m, Vector3 v);
Matrix4 m4Transpose(Matrix4 m);
Matrix4 m4NewScale(float x, float y, float z);
Matrix4 m4NewTranslate(float x, float y, float z);
Matrix4 m4NewRotateX(float angle);
Matrix4 m4NewRotateY(float angle);
Matrix4 m4NewRotateZ(float angle);
Matrix4 m4NewRotateAxis(float angle, Vector3 axis);
Matrix4 m4NewRotateEuler(float heading, float attitude, float bank);
Matrix4 m4NewRotateTripleAxis(Vector3 x, Vector3 y, Vector3 z);
Matrix4 m4NewLookAt(Vector3 eye, Vector3 at, Vector3 up);
Matrix4 m4NewPerspective(float fov_y, float aspect, float near, float far);
float m4Determinant(Matrix4 m);
Matrix4 m4Inverse(Matrix4 m);

#ifdef __cplusplus
}
#endif

#endif
