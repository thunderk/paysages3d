#include "euclid.h"

#include <math.h>
#include "../tools.h"

Vector3 VECTOR_ZERO = {0.0, 0.0, 0.0};
Vector3 VECTOR_DOWN = {0.0, -1.0, 0.0};
Vector3 VECTOR_UP = {0.0, 1.0, 0.0};
Vector3 VECTOR_NORTH = {0.0, 0.0, -1.0};
Vector3 VECTOR_SOUTH = {0.0, 0.0, 1.0};
Vector3 VECTOR_WEST = {-1.0, 0.0, 0.0};
Vector3 VECTOR_EAST = {1.0, 0.0, 0.0};

void v3Save(PackStream* stream, Vector3* v)
{
    packWriteDouble(stream, &v->x);
    packWriteDouble(stream, &v->y);
    packWriteDouble(stream, &v->z);
}

void v3Load(PackStream* stream, Vector3* v)
{
    packReadDouble(stream, &v->x);
    packReadDouble(stream, &v->y);
    packReadDouble(stream, &v->z);
}

Vector3 v3Translate(Vector3 v1, double x, double y, double z)
{
    Vector3 result;
    result.x = v1.x + x;
    result.y = v1.y + y;
    result.z = v1.z + z;
    return result;
}

Vector3 v3Add(Vector3 v1, Vector3 v2)
{
    Vector3 result;
    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;
    result.z = v1.z + v2.z;
    return result;
}

Vector3 v3Sub(Vector3 v1, Vector3 v2)
{
    Vector3 result;
    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    result.z = v1.z - v2.z;
    return result;
}

Vector3 v3Neg(Vector3 v)
{
    Vector3 result;
    result.x = -v.x;
    result.y = -v.y;
    result.z = -v.z;
    return result;
}

Vector3 v3Scale(Vector3 v, double scale)
{
    Vector3 result;
    result.x = v.x * scale;
    result.y = v.y * scale;
    result.z = v.z * scale;
    return result;
}

double v3Norm(Vector3 v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vector3 v3Normalize(Vector3 v)
{
    double norm = v3Norm(v);
    if (norm == 0.0)
    {
        return VECTOR_ZERO;
    }
    else
    {
        return v3Scale(v, 1.0 / norm);
    }
}

double v3Dot(Vector3 v1, Vector3 v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vector3 v3Cross(Vector3 v1, Vector3 v2)
{
    Vector3 result;
    result.x = v1.y * v2.z - v1.z * v2.y;
    result.y = v1.z * v2.x - v1.x * v2.z;
    result.z = v1.x * v2.y - v1.y * v2.x;
    return result;
}

void m4Save(PackStream* stream, Matrix4* m)
{
    packWriteDouble(stream, &m->a);
    packWriteDouble(stream, &m->b);
    packWriteDouble(stream, &m->c);
    packWriteDouble(stream, &m->d);
    packWriteDouble(stream, &m->e);
    packWriteDouble(stream, &m->f);
    packWriteDouble(stream, &m->g);
    packWriteDouble(stream, &m->h);
    packWriteDouble(stream, &m->i);
    packWriteDouble(stream, &m->j);
    packWriteDouble(stream, &m->k);
    packWriteDouble(stream, &m->l);
    packWriteDouble(stream, &m->m);
    packWriteDouble(stream, &m->n);
    packWriteDouble(stream, &m->o);
    packWriteDouble(stream, &m->p);
}

void m4Load(PackStream* stream, Matrix4* m)
{
    packReadDouble(stream, &m->a);
    packReadDouble(stream, &m->b);
    packReadDouble(stream, &m->c);
    packReadDouble(stream, &m->d);
    packReadDouble(stream, &m->e);
    packReadDouble(stream, &m->f);
    packReadDouble(stream, &m->g);
    packReadDouble(stream, &m->h);
    packReadDouble(stream, &m->i);
    packReadDouble(stream, &m->j);
    packReadDouble(stream, &m->k);
    packReadDouble(stream, &m->l);
    packReadDouble(stream, &m->m);
    packReadDouble(stream, &m->n);
    packReadDouble(stream, &m->o);
    packReadDouble(stream, &m->p);
}

Matrix4 m4NewIdentity()
{
    Matrix4 result;
    result.a = result.f = result.k = result.p = 1.0;
    result.b = result.c = result.d = result.e = result.g = result.h = 0.0;
    result.i = result.j = result.l = result.m = result.n = result.o = 0.0;
    return result;
}

Matrix4 m4Mult(Matrix4 m1, Matrix4 m2)
{
    Matrix4 result;
    result.a = m1.a * m2.a + m1.b * m2.e + m1.c * m2.i + m1.d * m2.m;
    result.b = m1.a * m2.b + m1.b * m2.f + m1.c * m2.j + m1.d * m2.n;
    result.c = m1.a * m2.c + m1.b * m2.g + m1.c * m2.k + m1.d * m2.o;
    result.d = m1.a * m2.d + m1.b * m2.h + m1.c * m2.l + m1.d * m2.p;
    result.e = m1.e * m2.a + m1.f * m2.e + m1.g * m2.i + m1.h * m2.m;
    result.f = m1.e * m2.b + m1.f * m2.f + m1.g * m2.j + m1.h * m2.n;
    result.g = m1.e * m2.c + m1.f * m2.g + m1.g * m2.k + m1.h * m2.o;
    result.h = m1.e * m2.d + m1.f * m2.h + m1.g * m2.l + m1.h * m2.p;
    result.i = m1.i * m2.a + m1.j * m2.e + m1.k * m2.i + m1.l * m2.m;
    result.j = m1.i * m2.b + m1.j * m2.f + m1.k * m2.j + m1.l * m2.n;
    result.k = m1.i * m2.c + m1.j * m2.g + m1.k * m2.k + m1.l * m2.o;
    result.l = m1.i * m2.d + m1.j * m2.h + m1.k * m2.l + m1.l * m2.p;
    result.m = m1.m * m2.a + m1.n * m2.e + m1.o * m2.i + m1.p * m2.m;
    result.n = m1.m * m2.b + m1.n * m2.f + m1.o * m2.j + m1.p * m2.n;
    result.o = m1.m * m2.c + m1.n * m2.g + m1.o * m2.k + m1.p * m2.o;
    result.p = m1.m * m2.d + m1.n * m2.h + m1.o * m2.l + m1.p * m2.p;
    return result;
}

Vector3 m4MultPoint(Matrix4 m, Vector3 v)
{
    Vector3 result;
    result.x = m.a * v.x + m.b * v.y + m.c * v.z + m.d;
    result.y = m.e * v.x + m.f * v.y + m.g * v.z + m.h;
    result.z = m.i * v.x + m.j * v.y + m.k * v.z + m.l;
    return result;
}

Vector3 m4Transform(Matrix4 m, Vector3 v)
{
    Vector3 result;
    double w;
    result.x = m.a * v.x + m.b * v.y + m.c * v.z + m.d;
    result.y = m.e * v.x + m.f * v.y + m.g * v.z + m.h;
    result.z = m.i * v.x + m.j * v.y + m.k * v.z + m.l;
    w = m.m * v.x + m.n * v.y + m.o * v.z + m.p;
    if (w != 0.0)
    {
        result.x /= w;
        result.y /= w;
        result.z /= w;
    }
    return result;
}

Matrix4 m4Transpose(Matrix4 m)
{
    Matrix4 result;
    result.a = m.a;
    result.e = m.b;
    result.i = m.c;
    result.m = m.d;
    result.b = m.e;
    result.f = m.f;
    result.j = m.g;
    result.n = m.h;
    result.c = m.i;
    result.g = m.j;
    result.k = m.k;
    result.o = m.l;
    result.d = m.m;
    result.h = m.n;
    result.l = m.o;
    result.p = m.p;
    return result;
}

Matrix4 m4NewScale(double x, double y, double z)
{
    Matrix4 result = m4NewIdentity();
    result.a = x;
    result.f = y;
    result.k = z;
    return result;
}

Matrix4 m4NewTranslate(double x, double y, double z)
{
    Matrix4 result = m4NewIdentity();
    result.d = x;
    result.h = y;
    result.l = z;
    return result;
}

Matrix4 m4NewRotateX(double angle)
{
    Matrix4 result = m4NewIdentity();
    double s = sin(angle);
    double c = cos(angle);
    result.f = result.k = c;
    result.g = -s;
    result.j = s;
    return result;
}

Matrix4 m4NewRotateY(double angle)
{
    Matrix4 result = m4NewIdentity();
    double s = sin(angle);
    double c = cos(angle);
    result.a = result.k = c;
    result.c = s;
    result.i = -s;
    return result;
}

Matrix4 m4NewRotateZ(double angle)
{
    Matrix4 result = m4NewIdentity();
    double s = sin(angle);
    double c = cos(angle);
    result.a = result.f = c;
    result.b = -s;
    result.e = s;
    return result;
}

Matrix4 m4NewRotateAxis(double angle, Vector3 axis)
{
    Matrix4 result = m4NewIdentity();
    double s = sin(angle);
    double c = cos(angle);
    double c1 = 1.0 - c;
    axis = v3Normalize(axis);
    result.a = axis.x * axis.x * c1 + c;
    result.b = axis.x * axis.y * c1 - axis.z * s;
    result.c = axis.x * axis.z * c1 + axis.y * s;
    result.e = axis.y * axis.x * c1 + axis.z * s;
    result.f = axis.y * axis.y * c1 + c;
    result.g = axis.y * axis.z * c1 - axis.x * s;
    result.i = axis.x * axis.z * c1 - axis.y * s;
    result.j = axis.y * axis.z * c1 + axis.x * s;
    result.k = axis.z * axis.z * c1 + c;
    return result;
}

Matrix4 m4NewRotateEuler(double heading, double attitude, double bank)
{
    Matrix4 result = m4NewIdentity();
    double ch = cos(heading);
    double sh = sin(heading);
    double ca = cos(attitude);
    double sa = sin(attitude);
    double cb = cos(bank);
    double sb = sin(bank);
    result.a = ch * ca;
    result.b = sh * sb - ch * sa * cb;
    result.c = ch * sa * sb + sh * cb;
    result.e = sa;
    result.f = ca * cb;
    result.g = -ca * sb;
    result.i = -sh * ca;
    result.j = sh * sa * cb + ch * sb;
    result.k = -sh * sa * sb + ch * cb;
    return result;
}

Matrix4 m4NewRotateTripleAxis(Vector3 x, Vector3 y, Vector3 z)
{
    Matrix4 result = m4NewIdentity();
    result.a = x.x;
    result.b = y.x;
    result.c = z.x;
    result.e = x.y;
    result.f = y.y;
    result.g = z.y;
    result.i = x.z;
    result.j = y.z;
    result.k = z.z;
    return result;
}

Matrix4 m4NewLookAt(Vector3 eye, Vector3 at, Vector3 up)
{
    Vector3 z = v3Normalize(v3Sub(at, eye));
    Vector3 x = v3Normalize(v3Cross(up, z));
    Vector3 y = v3Cross(z, x);
    Matrix4 result = m4NewRotateTripleAxis(x, y, z);
    result.d = eye.x;
    result.h = eye.y;
    result.l = eye.z;
    return m4Inverse(result);
}

Matrix4 m4NewPerspective(double fov_y, double aspect, double near, double far)
{
    Matrix4 result = m4NewIdentity();
    double f = 1 / tan(fov_y / 2.0);
    result.a = f / aspect;
    result.f = f;
    result.k = (far + near) / (near - far);
    result.l = 2.0 * far * near / (near - far);
    result.o = -1.0;
    result.p = 0.0;
    return result;
}

double m4Determinant(Matrix4 m)
{
    return ((m.a * m.f - m.e * m.b)
          * (m.k * m.p - m.o * m.l)
          - (m.a * m.j - m.i * m.b)
          * (m.g * m.p - m.o * m.h)
          + (m.a * m.n - m.m * m.b)
          * (m.g * m.l - m.k * m.h)
          + (m.e * m.j - m.i * m.f)
          * (m.c * m.p - m.o * m.d)
          - (m.e * m.n - m.m * m.f)
          * (m.c * m.l - m.k * m.d)
          + (m.i * m.n - m.m * m.j)
          * (m.c * m.h - m.g * m.d));
}

Matrix4 m4Inverse(Matrix4 m)
{
    Matrix4 result;
    double d = m4Determinant(m);

    if (fabs(d) < 0.00001)
    {
        return m4NewIdentity();
    }
    else
    {
        d = 1.0 / d;

        result.a = d * (m.f * (m.k * m.p - m.o * m.l) + m.j * (m.o * m.h - m.g * m.p) + m.n * (m.g * m.l - m.k * m.h));
        result.e = d * (m.g * (m.i * m.p - m.m * m.l) + m.k * (m.m * m.h - m.e * m.p) + m.o * (m.e * m.l - m.i * m.h));
        result.i = d * (m.h * (m.i * m.n - m.m * m.j) + m.l * (m.m * m.f - m.e * m.n) + m.p * (m.e * m.j - m.i * m.f));
        result.m = d * (m.e * (m.n * m.k - m.j * m.o) + m.i * (m.f * m.o - m.n * m.g) + m.m * (m.j * m.g - m.f * m.k));

        result.b = d * (m.j * (m.c * m.p - m.o * m.d) + m.n * (m.k * m.d - m.c * m.l) + m.b * (m.o * m.l - m.k * m.p));
        result.f = d * (m.k * (m.a * m.p - m.m * m.d) + m.o * (m.i * m.d - m.a * m.l) + m.c * (m.m * m.l - m.i * m.p));
        result.j = d * (m.l * (m.a * m.n - m.m * m.b) + m.p * (m.i * m.b - m.a * m.j) + m.d * (m.m * m.j - m.i * m.n));
        result.n = d * (m.i * (m.n * m.c - m.b * m.o) + m.m * (m.b * m.k - m.j * m.c) + m.a * (m.j * m.o - m.n * m.k));

        result.c = d * (m.n * (m.c * m.h - m.g * m.d) + m.b * (m.g * m.p - m.o * m.h) + m.f * (m.o * m.d - m.c * m.p));
        result.g = d * (m.o * (m.a * m.h - m.e * m.d) + m.c * (m.e * m.p - m.m * m.h) + m.g * (m.m * m.d - m.a * m.p));
        result.k = d * (m.p * (m.a * m.f - m.e * m.b) + m.d * (m.e * m.n - m.m * m.f) + m.h * (m.m * m.b - m.a * m.n));
        result.o = d * (m.m * (m.f * m.c - m.b * m.g) + m.a * (m.n * m.g - m.f * m.o) + m.e * (m.b * m.o - m.n * m.c));

        result.d = d * (m.b * (m.k * m.h - m.g * m.l) + m.f * (m.c * m.l - m.k * m.d) + m.j * (m.g * m.d - m.c * m.h));
        result.h = d * (m.c * (m.i * m.h - m.e * m.l) + m.g * (m.a * m.l - m.i * m.d) + m.k * (m.e * m.d - m.a * m.h));
        result.l = d * (m.d * (m.i * m.f - m.e * m.j) + m.h * (m.a * m.j - m.i * m.b) + m.l * (m.e * m.b - m.a * m.f));
        result.p = d * (m.a * (m.f * m.k - m.j * m.g) + m.e * (m.j * m.c - m.b * m.k) + m.i * (m.b * m.g - m.f * m.c));

        return result;
    }
}

