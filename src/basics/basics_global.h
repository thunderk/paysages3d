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
    class SpaceSegment;
}
}
using namespace paysages::basics;

#endif // BASICS_GLOBAL_H
