#include "Vector3.h"

#include <math.h>
#include "PackStream.h"

Vector3 VECTOR_ZERO = {0.0, 0.0, 0.0};
Vector3 VECTOR_DOWN = {0.0, -1.0, 0.0};
Vector3 VECTOR_UP = {0.0, 1.0, 0.0};
Vector3 VECTOR_NORTH = {0.0, 0.0, -1.0};
Vector3 VECTOR_SOUTH = {0.0, 0.0, 1.0};
Vector3 VECTOR_WEST = {-1.0, 0.0, 0.0};
Vector3 VECTOR_EAST = {1.0, 0.0, 0.0};

static inline double _euclidGet2DAngle(double x, double y)
{
    // TEMP Copy of old euclid.c
    double nx, ny, d, ret;

    if (x == 0.0)
    {
        if (y == 0.0)
        {
            return 0.0;
        }
        else if (y < 0.0)
        {
            return 3.0 * M_PI_2;
        }
        else
        {
            return M_PI_2;
        }
    }

    d = sqrt(x * x + y * y);
    nx = x / d;
    ny = y / d;

    ret = asin(ny);
    if (nx < 0.0)
    {
        ret = M_PI - ret;
    }
    return ret < 0.0 ? ret + 2.0 * M_PI : ret;
}
void v3Save(PackStream* stream, Vector3* v)
{
    stream->write(&v->x);
    stream->write(&v->y);
    stream->write(&v->z);
}

void v3Load(PackStream* stream, Vector3* v)
{
    stream->read(&v->x);
    stream->read(&v->y);
    stream->read(&v->z);
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

VectorSpherical v3ToSpherical(Vector3 v)
{
    VectorSpherical result;

    result.phi = _euclidGet2DAngle(v.x, -v.z);
    result.theta = _euclidGet2DAngle(sqrt(v.x * v.x + v.z * v.z), v.y);
    if (v.y < 0.0)
    {
        result.theta -= 2.0 * M_PI;
    }
    result.r = v3Norm(v);

    return result;
}

Vector3 v3FromSpherical(VectorSpherical v)
{
    Vector3 result = {v.r * cos(v.phi) * cos(v.theta), v.r * sin(v.theta), -v.r * sin(v.phi) * cos(v.theta)};
    return result;
}
