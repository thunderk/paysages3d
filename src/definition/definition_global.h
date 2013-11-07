#ifndef DEFINITION_GLOBAL_H
#define DEFINITION_GLOBAL_H

/* Shared object helpers */
#include <QtCore/qglobal.h>
#if defined(DEFINITION_LIBRARY)
#  define DEFINITIONSHARED_EXPORT Q_DECL_EXPORT
#else
#  define DEFINITIONSHARED_EXPORT Q_DECL_IMPORT
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

#endif // DEFINITION_GLOBAL_H
