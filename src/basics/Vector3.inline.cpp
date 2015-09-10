#define VECTOR3_INLINE_CPP

#ifdef VECTOR3_H
# define METHSPEC inline
#else
# include "Vector3.h"
# define METHSPEC
#endif

#include <cmath>

METHSPEC Vector3::Vector3(double x, double y, double z):
    x(x), y(y), z(z)
{
}

METHSPEC Vector3 Vector3::add(double x, double y, double z) const
{
    return Vector3(this->x + x, this->y + y, this->z + z);
}

METHSPEC Vector3 Vector3::add(const Vector3 &other) const
{
    return Vector3(x + other.x, y + other.y, z + other.z);
}

METHSPEC Vector3 Vector3::sub(const Vector3 &other) const
{
    return Vector3(x - other.x, y - other.y, z - other.z);
}

METHSPEC Vector3 Vector3::inverse() const
{
    return Vector3(-x, -y, -z);
}

METHSPEC Vector3 Vector3::scale(double scaling) const
{
    return Vector3(x * scaling, y * scaling, z * scaling);
}

METHSPEC double Vector3::getNorm() const
{
    return sqrt(x * x + y * y + z * z);
}

METHSPEC Vector3 Vector3::normalize() const
{
    double norm = sqrt(x * x + y * y + z * z);
    if (norm == 0.0)
    {
        return VECTOR_ZERO;
    }
    else
    {
        norm = 1.0 / norm;
        return Vector3(x * norm, y * norm, z * norm);
    }
}

METHSPEC double Vector3:: dotProduct(const Vector3 &other) const
{
    return x * other.x + y * other.y + z * other.z;
}

METHSPEC Vector3 Vector3::crossProduct(const Vector3 &other) const
{
    return Vector3(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    );
}
