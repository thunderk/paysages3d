#ifndef RENDERING_GLOBAL_H
#define RENDERING_GLOBAL_H

/* Shared object helpers */
#include <QtCore/qglobal.h>
#if defined(RENDERING_LIBRARY)
#  define RENDERINGSHARED_EXPORT Q_DECL_EXPORT
#else
#  define RENDERINGSHARED_EXPORT Q_DECL_IMPORT
#endif

/* Namespace using */
namespace paysages
{
    namespace system {}
    namespace basics {}
    namespace definition {}
}
using namespace paysages::system;
using namespace paysages::basics;
using namespace paysages::definition;

#endif // RENDERING_GLOBAL_H
