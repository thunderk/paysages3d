#ifndef SYSTEM_GLOBAL_H
#define SYSTEM_GLOBAL_H

#ifndef __MINGW32__
#define PAYSAGES_USE_INLINING 1
#endif

#include <QtCore/qglobal.h>
#if defined(SYSTEM_LIBRARY)
#  define SYSTEMSHARED_EXPORT Q_DECL_EXPORT
#else
#  define SYSTEMSHARED_EXPORT Q_DECL_IMPORT
#endif

namespace paysages {
namespace system {
    class PackStream;
    class ParallelWork;
    class ParallelPool;
    class ParallelWorker;
    class Thread;
    class Mutex;
    class Semaphore;
}
}
using namespace paysages::system;

#endif // SYSTEM_GLOBAL_H
