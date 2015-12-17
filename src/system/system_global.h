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

extern SYSTEMSHARED_EXPORT RandomGenerator &RandomGeneratorDefault;
}
}
using namespace paysages::system;

using namespace std;

// Some useful casts
#define to_double(_x_) (static_cast<double>(_x_))
#define trunc_to_int(_x_) (static_cast<int>(_x_))
#define round_to_int(_x_) (static_cast<int>(round(_x_)))
#define floor_to_int(_x_) (static_cast<int>(floor(_x_)))
#define ceil_to_int(_x_) (static_cast<int>(ceil(_x_)))
#define to_size(_x_) (static_cast<unsigned long>(_x_))

#endif // SYSTEM_GLOBAL_H
