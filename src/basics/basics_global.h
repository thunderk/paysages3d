#ifndef BASICS_GLOBAL_H
#define BASICS_GLOBAL_H

/* Shared object helpers */
#ifdef __cplusplus
#  include <QtCore/qglobal.h>
#else
#  if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#    define Q_DECL_EXPORT __declspec(dllexport)
#    define Q_DECL_IMPORT __declspec(dllimport)
#  else
#    define Q_DECL_EXPORT
#    define Q_DECL_IMPORT
#  endif
#endif
#if defined(DEFINITION_LIBRARY)
#  define BASICSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define BASICSSHARED_EXPORT Q_DECL_IMPORT
#endif

/* Namespace using */
#ifdef __cplusplus
namespace paysages
{
    namespace basics {}
}
using namespace paysages::system;
#endif

/* Global imports */

#endif // BASICS_GLOBAL_H
