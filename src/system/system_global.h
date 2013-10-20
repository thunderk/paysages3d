#ifndef SYSTEM_GLOBAL_H
#define SYSTEM_GLOBAL_H

/* Shared object helpers */
#ifdef __cplusplus
#  include <QtCore/qglobal.h>
#  if defined(SYSTEM_LIBRARY)
#    define SYSTEMSHARED_EXPORT Q_DECL_EXPORT
#  else
#    define SYSTEMSHARED_EXPORT Q_DECL_IMPORT
#  endif
#else
#  if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#    define SYSTEMSHARED_EXPORT __declspec(dllimport)
#  else
#    define SYSTEMSHARED_EXPORT
#  endif
#endif

/* Namespace using */
#ifdef __cplusplus
namespace paysages
{
    namespace system {}
}
using namespace paysages::system;
#endif

/* Global import */

#endif // SYSTEM_GLOBAL_H
