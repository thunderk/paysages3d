#ifndef SYSTEM_GLOBAL_H
#define SYSTEM_GLOBAL_H

#define PAYSAGES_USE_INLINING 1

#include <QtCore/qglobal.h>
#if defined(SYSTEM_LIBRARY)
#  define SYSTEMSHARED_EXPORT Q_DECL_EXPORT
#else
#  define SYSTEMSHARED_EXPORT Q_DECL_IMPORT
#endif

namespace paysages {
namespace system {
    class PackStream;
    class ParallelQueue;
    class ParallelWork;
    class Thread;
    class Mutex;
}
}
using namespace paysages::system;

#endif // SYSTEM_GLOBAL_H
