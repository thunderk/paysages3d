#ifndef SYSTEM_GLOBAL_H
#define SYSTEM_GLOBAL_H

#if defined(__MINGW32__) || defined(_WIN32)
#define DECL_EXPORT __declspec(dllexport)
#define DECL_IMPORT __declspec(dllimport)
#define PLATFORM_WIN 1
#else
#define DECL_EXPORT __attribute__((visibility("default")))
#define DECL_IMPORT __attribute__((visibility("default")))
#define PLATFORM_UNIX 1
#endif

#if defined(SYSTEM_LIBRARY)
#define SYSTEMSHARED_EXPORT DECL_EXPORT
#else
#define SYSTEMSHARED_EXPORT DECL_IMPORT
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
class Timing;
class RandomGenerator;

extern SYSTEMSHARED_EXPORT RandomGenerator &RandomGeneratorDefault;
}
}
using namespace paysages::system;

using namespace std;

// Some useful casts
#define to_double(_x_) (static_cast<double>(_x_))
#define to_float(_x_) (static_cast<float>(_x_))
#define trunc_to_int(_x_) (static_cast<int>(_x_))
#define round_to_int(_x_) (static_cast<int>(round(_x_)))
#define floor_to_int(_x_) (static_cast<int>(floor(_x_)))
#define ceil_to_int(_x_) (static_cast<int>(ceil(_x_)))
#define to_size(_x_) (static_cast<unsigned long>(_x_))

#endif // SYSTEM_GLOBAL_H
