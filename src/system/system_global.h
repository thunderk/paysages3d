#ifndef SYSTEM_GLOBAL_H
#define SYSTEM_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SYSTEM_LIBRARY)
#  define SYSTEMSHARED_EXPORT Q_DECL_EXPORT
#else
#  define SYSTEMSHARED_EXPORT Q_DECL_IMPORT
#endif

#ifdef __cplusplus
namespace paysages
{
    namespace system {}
}

using namespace paysages::system;
#endif

#endif // SYSTEM_GLOBAL_H
