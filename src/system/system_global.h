#ifndef SYSTEM_GLOBAL_H
#define SYSTEM_GLOBAL_H

#include <string>

#ifdef __MINGW32__
#define Q_DECL_EXPORT __declspec(dllexport)
#define Q_DECL_IMPORT __declspec(dllimport)
#else
#define Q_DECL_EXPORT __attribute__((visibility("default")))
#define Q_DECL_IMPORT __attribute__((visibility("default")))
#endif

#if defined(SYSTEM_LIBRARY)
#define SYSTEMSHARED_EXPORT Q_DECL_EXPORT
#else
#define SYSTEMSHARED_EXPORT Q_DECL_IMPORT
#endif

namespace paysages {
namespace system {
class Logs;
class PackStream;
class ParallelWork;
class ParallelPool;
class ParallelWorker;
class Thread;
class Mutex;
class Semaphore;
class PictureWriter;
class Time;
class RandomGenerator;

extern RandomGenerator &RandomGeneratorDefault;
}
}
using namespace paysages::system;

using namespace std;

#endif // SYSTEM_GLOBAL_H
