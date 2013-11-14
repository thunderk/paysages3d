#include "euclid.h"

#include <math.h>
#include "PackStream.h"

double euclidGet2DAngle(double x, double y)
{
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

Vector3 euclidGetNormalFromTriangle(Vector3 center, Vector3 bottom, Vector3 right)
{
    Vector3 dx = v3Sub(right, center);
    Vector3 dz = v3Sub(bottom, center);
    return v3Normalize(v3Cross(dz, dx));
}

double euclidGetDistance2D(double x1, double y1, double x2, double y2)
{
    double dx = x2 - x1;
    double dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

int euclidRayIntersectSphere(Vector3 ray_point, Vector3 ray_direction, Vector3 sphere_center, double sphere_radius, Vector3* hit1, Vector3* hit2)
{
    Vector3 ray_direction_sphere;
    double a, b, c, d;

    ray_direction_sphere = v3Sub(ray_point, sphere_center);

    a = ray_direction.x * ray_direction.x + ray_direction.y * ray_direction.y + ray_direction.z * ray_direction.z;
    b = 2 * (ray_direction.x * ray_direction_sphere.x + ray_direction.y * ray_direction_sphere.y + ray_direction.z * ray_direction_sphere.z);
    c = ray_direction_sphere.x * ray_direction_sphere.x + ray_direction_sphere.y * ray_direction_sphere.y + ray_direction_sphere.z * ray_direction_sphere.z - sphere_radius * sphere_radius;
    d = b * b - 4 * a * c;

    if (d < 0.0)
    {
        return 0;
    }
    else if (d > 0.0)
    {
        if (hit1 && hit2)
        {
            *hit1 = v3Add(ray_point, v3Scale(ray_direction, (-b - sqrt(d)) / (2 * a)));
            *hit2 = v3Add(ray_point, v3Scale(ray_direction, (-b + sqrt(d)) / (2 * a)));
        }
        return 2;
    }
    else
    {
        if (hit1)
        {
            *hit1 = v3Add(ray_point, v3Scale(ray_direction, -b / (2 * a)));
        }
        return 1;
    }
}
