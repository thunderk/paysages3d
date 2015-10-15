#ifndef BASICS_GLOBAL_H
#define BASICS_GLOBAL_H

#include <QtCore/qglobal.h>
#if defined(BASICS_LIBRARY)
#  define BASICSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define BASICSSHARED_EXPORT Q_DECL_IMPORT
#endif

#include "system_global.h"

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
}
}
using namespace paysages::basics;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif
#ifndef M_PI_4
#define M_PI_4 0.78539816339744830962
#endif
#ifndef M_2PI
#define M_2PI 6.28318530717958647692
#endif

#endif // BASICS_GLOBAL_H
