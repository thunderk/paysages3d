#ifndef BASICS_GLOBAL_H
#define BASICS_GLOBAL_H

/* Shared object helpers */
#include <QtCore/qglobal.h>
#if defined(BASICS_LIBRARY)
#  define BASICSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define BASICSSHARED_EXPORT Q_DECL_IMPORT
#endif

/* Namespace using */
namespace paysages
{
    namespace system {}
    namespace basics {}
}
using namespace paysages::system;
using namespace paysages::basics;

#endif // BASICS_GLOBAL_H
