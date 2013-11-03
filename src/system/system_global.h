#ifndef SYSTEM_GLOBAL_H
#define SYSTEM_GLOBAL_H

/* Shared object helpers */
#include <QtCore/qglobal.h>
#if defined(SYSTEM_LIBRARY)
#  define SYSTEMSHARED_EXPORT Q_DECL_EXPORT
#else
#  define SYSTEMSHARED_EXPORT Q_DECL_IMPORT
#endif

/* Namespace using */
namespace paysages
{
    namespace system {}
}
using namespace paysages::system;

#endif // SYSTEM_GLOBAL_H
