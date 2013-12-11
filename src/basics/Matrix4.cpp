#include "Matrix4.h"

#include <cmath>
#include "PackStream.h"

Matrix4::Matrix4(bool identity)
{
    b = c = d = e = g = h = 0.0;
    i = j = l = m = n = o = 0.0;
    a = f = k = p = (identity ? 1.0 : 0.0);
}

void Matrix4::save(PackStream* stream) const
{
    stream->write(&a);
    stream->write(&b);
    stream->write(&c);
    stream->write(&d);
    stream->write(&e);
    stream->write(&f);
    stream->write(&g);
    stream->write(&h);
    stream->write(&i);
    stream->write(&j);
    stream->write(&k);
    stream->write(&l);
    stream->write(&m);
    stream->write(&n);
    stream->write(&o);
    stream->write(&p);
}

void Matrix4::load(PackStream* stream)
{
    stream->read(&a);
    stream->read(&b);
    stream->read(&c);
    stream->read(&d);
    stream->read(&e);
    stream->read(&f);
    stream->read(&g);
    stream->read(&h);
    stream->read(&i);
    stream->read(&j);
    stream->read(&k);
    stream->read(&l);
    stream->read(&m);
    stream->read(&n);
    stream->read(&o);
    stream->read(&p);
}

Matrix4 Matrix4::mult(const Matrix4 &other) const
{
    Matrix4 result;
    result.a = a * other.a + b * other.e + c * other.i + d * other.m;
    result.b = a * other.b + b * other.f + c * other.j + d * other.n;
    result.c = a * other.c + b * other.g + c * other.k + d * other.o;
    result.d = a * other.d + b * other.h + c * other.l + d * other.p;
    result.e = e * other.a + f * other.e + g * other.i + h * other.m;
    result.f = e * other.b + f * other.f + g * other.j + h * other.n;
    result.g = e * other.c + f * other.g + g * other.k + h * other.o;
    result.h = e * other.d + f * other.h + g * other.l + h * other.p;
    result.i = i * other.a + j * other.e + k * other.i + l * other.m;
    result.j = i * other.b + j * other.f + k * other.j + l * other.n;
    result.k = i * other.c + j * other.g + k * other.k + l * other.o;
    result.l = i * other.d + j * other.h + k * other.l + l * other.p;
    result.m = m * other.a + n * other.e + o * other.i + p * other.m;
    result.n = m * other.b + n * other.f + o * other.j + p * other.n;
    result.o = m * other.c + n * other.g + o * other.k + p * other.o;
    result.p = m * other.d + n * other.h + o * other.l + p * other.p;
    return result;
}

Vector3 Matrix4::multPoint(const Vector3 &v) const
{
    Vector3 result;
    result.x = a * v.x + b * v.y + c * v.z + d;
    result.y = e * v.x + f * v.y + g * v.z + h;
    result.z = i * v.x + j * v.y + k * v.z + l;
    return result;
}

Vector3 Matrix4::transform(const Vector3 &v) const
{
    double w = m * v.x + n * v.y + o * v.z + p;
    if (w != 0.0)
    {
        w = 1.0 / w;
        return Vector3(
            (a * v.x + b * v.y + c * v.z + d) * w,
            (e * v.x + f * v.y + g * v.z + h) * w,
            (i * v.x + j * v.y + k * v.z + l) * w
        );
    }
    else
    {
        return Vector3(
            a * v.x + b * v.y + c * v.z + d,
            e * v.x + f * v.y + g * v.z + h,
            i * v.x + j * v.y + k * v.z + l
        );
    }
}

Matrix4 Matrix4::transposed() const
{
    Matrix4 result;
    result.a = a;
    result.e = b;
    result.i = c;
    result.m = d;
    result.b = e;
    result.f = f;
    result.j = g;
    result.n = h;
    result.c = i;
    result.g = j;
    result.k = k;
    result.o = l;
    result.d = m;
    result.h = n;
    result.l = o;
    result.p = p;
    return result;
}

Matrix4 Matrix4::newScale(double x, double y, double z)
{
    Matrix4 result;
    result.a = x;
    result.f = y;
    result.k = z;
    return result;
}

Matrix4 Matrix4::newTranslate(double x, double y, double z)
{
    Matrix4 result;
    result.d = x;
    result.h = y;
    result.l = z;
    return result;
}

Matrix4 Matrix4::newRotateX(double angle)
{
    Matrix4 result;
    double si = sin(angle);
    double co = cos(angle);
    result.f = result.k = co;
    result.g = -si;
    result.j = si;
    return result;
}

Matrix4 Matrix4::newRotateY(double angle)
{
    Matrix4 result;
    double si = sin(angle);
    double co = cos(angle);
    result.a = result.k = co;
    result.c = si;
    result.i = -si;
    return result;
}

Matrix4 Matrix4::newRotateZ(double angle)
{
    Matrix4 result;
    double si = sin(angle);
    double co = cos(angle);
    result.a = result.f = co;
    result.b = -si;
    result.e = si;
    return result;
}

Matrix4 Matrix4::newRotateAxis(double angle, const Vector3 &_axis)
{
    Matrix4 result;
    double si = sin(angle);
    double co = cos(angle);
    double c1 = 1.0 - co;
    Vector3 axis = _axis.normalize();
    result.a = axis.x * axis.x * c1 + co;
    result.b = axis.x * axis.y * c1 - axis.z * si;
    result.c = axis.x * axis.z * c1 + axis.y * si;
    result.e = axis.y * axis.x * c1 + axis.z * si;
    result.f = axis.y * axis.y * c1 + co;
    result.g = axis.y * axis.z * c1 - axis.x * si;
    result.i = axis.x * axis.z * c1 - axis.y * si;
    result.j = axis.y * axis.z * c1 + axis.x * si;
    result.k = axis.z * axis.z * c1 + co;
    return result;
}

Matrix4 Matrix4::newRotateEuler(double heading, double attitude, double bank)
{
    Matrix4 result;
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

Matrix4 Matrix4::newRotateTripleAxis(const Vector3 &x, const Vector3 &y, const Vector3 &z)
{
    Matrix4 result;
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

Matrix4 Matrix4::newLookAt(const Vector3 &eye, const Vector3 &at, const Vector3 &up)
{
    Vector3 z = at.sub(eye).normalize();
    Vector3 x = up.crossProduct(z).normalize();
    Vector3 y = z.crossProduct(x);
    Matrix4 result = Matrix4::newRotateTripleAxis(x, y, z);
    result.d = eye.x;
    result.h = eye.y;
    result.l = eye.z;
    return result.inversed();
}

Matrix4 Matrix4::newPerspective(double fov_y, double aspect, double near, double far)
{
    Matrix4 result;
    double fo = 1 / tan(fov_y / 2.0);
    result.a = fo / aspect;
    result.f = fo;
    result.k = (far + near) / (near - far);
    result.l = 2.0 * far * near / (near - far);
    result.o = -1.0;
    result.p = 0.0;
    return result;
}

double Matrix4::getDeterminant() const
{
    return ((a * f - e * b)
            * (k * p - o * l)
            - (a * j - i * b)
            * (g * p - o * h)
            + (a * n - m * b)
            * (g * l - k * h)
            + (e * j - i * f)
            * (c * p - o * d)
            - (e * n - m * f)
            * (c * l - k * d)
            + (i * n - m * j)
            * (c * h - g * d));
}

Matrix4 Matrix4::inversed() const
{
    Matrix4 result;
    double det = getDeterminant();

    if (fabs(det) >= 0.00001)
    {
        det = 1.0 / det;

        result.a = det * (f * (k * p - o * l) + j * (o * h - g * p) + n * (g * l - k * h));
        result.e = det * (g * (i * p - m * l) + k * (m * h - e * p) + o * (e * l - i * h));
        result.i = det * (h * (i * n - m * j) + l * (m * f - e * n) + p * (e * j - i * f));
        result.m = det * (e * (n * k - j * o) + i * (f * o - n * g) + m * (j * g - f * k));

        result.b = det * (j * (c * p - o * d) + n * (k * d - c * l) + b * (o * l - k * p));
        result.f = det * (k * (a * p - m * d) + o * (i * d - a * l) + c * (m * l - i * p));
        result.j = det * (l * (a * n - m * b) + p * (i * b - a * j) + d * (m * j - i * n));
        result.n = det * (i * (n * c - b * o) + m * (b * k - j * c) + a * (j * o - n * k));

        result.c = det * (n * (c * h - g * d) + b * (g * p - o * h) + f * (o * d - c * p));
        result.g = det * (o * (a * h - e * d) + c * (e * p - m * h) + g * (m * d - a * p));
        result.k = det * (p * (a * f - e * b) + d * (e * n - m * f) + h * (m * b - a * n));
        result.o = det * (m * (f * c - b * g) + a * (n * g - f * o) + e * (b * o - n * c));

        result.d = det * (b * (k * h - g * l) + f * (c * l - k * d) + j * (g * d - c * h));
        result.h = det * (c * (i * h - e * l) + g * (a * l - i * d) + k * (e * d - a * h));
        result.l = det * (d * (i * f - e * j) + h * (a * j - i * b) + l * (e * b - a * f));
        result.p = det * (a * (f * k - j * g) + e * (j * c - b * k) + i * (b * g - f * c));

    }

    return result;
}
