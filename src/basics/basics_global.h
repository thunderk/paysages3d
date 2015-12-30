#ifndef BASICS_GLOBAL_H
#define BASICS_GLOBAL_H

#include "system_global.h"

#if defined(BASICS_LIBRARY)
#define BASICSSHARED_EXPORT DECL_EXPORT
#else
#define BASICSSHARED_EXPORT DECL_IMPORT
#endif

namespace paysages {
namespace basics {
class Vector3;
class Matrix4;
class BoundingBox;
class SpaceGridIterator;
class SpaceSegment;
class Color;
class NoiseGenerator;
class NoiseState;
class FractalNoise;
class Curve;
class ColorProfile;
class Texture2D;
class Texture3D;
class Texture4D;
class CappedCylinder;
class InfiniteRay;
class Sphere;
class InfinitePlane;
}
}
using namespace paysages::basics;

#endif // BASICS_GLOBAL_H
