#ifndef _PAYSAGES_TOOLS_EUCLID_H_
#define _PAYSAGES_TOOLS_EUCLID_H_

#include "../rendering_global.h"

// TEMP
#include "Vector3.h"

namespace paysages {
namespace system {class PackStream;}
}

RENDERINGSHARED_EXPORT double euclidGet2DAngle(double x, double y);
RENDERINGSHARED_EXPORT Vector3 euclidGetNormalFromTriangle(Vector3 center, Vector3 bottom, Vector3 right);
RENDERINGSHARED_EXPORT double euclidGetDistance2D(double x1, double y1, double x2, double y2);
RENDERINGSHARED_EXPORT int euclidRayIntersectSphere(Vector3 ray_point, Vector3 ray_direction, Vector3 sphere_center, double sphere_radius, Vector3* hit1, Vector3* hit2);

#endif
