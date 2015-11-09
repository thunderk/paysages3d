#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "basics_global.h"

namespace paysages {
namespace basics {

class BASICSSHARED_EXPORT Geometry {
  public:
    static double get2DAngle(double x, double y);
    static Vector3 getNormalFromTriangle(const Vector3 &center, const Vector3 &bottom, const Vector3 &right);
    static double getDistance2D(double x1, double y1, double x2, double y2);
    static int rayIntersectSphere(const Vector3 &ray_point, const Vector3 &ray_direction, const Vector3 &sphere_center,
                                  double sphere_radius, Vector3 *hit1, Vector3 *hit2);
};
}
}

#endif // GEOMETRY_H
