#ifndef SOFTWARE_GLOBAL_H
#define SOFTWARE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SOFTWARE_LIBRARY)
#  define SOFTWARESHARED_EXPORT Q_DECL_EXPORT
#else
#  define SOFTWARESHARED_EXPORT Q_DECL_IMPORT
#endif

namespace paysages {
namespace basics {}
namespace definition {}
namespace software {}
}

using namespace paysages::basics;
using namespace paysages::definition;
using namespace paysages::software;

#endif // SOFTWARE_GLOBAL_H
