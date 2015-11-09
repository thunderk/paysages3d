#ifndef PARALLELWORK_H
#define PARALLELWORK_H

#include "system_global.h"

#define PARALLEL_MAX_THREADS 20

namespace paysages {
namespace system {

class SYSTEMSHARED_EXPORT ParallelWork {
  public:
    typedef int (*ParallelUnitFunction)(ParallelWork *work, int unit, void *data);

    /**
     * Obscure thread class.
     */
    class ParallelThread;
    friend class ParallelThread;

  public:
    /**
     * Create a parallel work handler.
     *
     * This will spawn a number of threads.
     */
    ParallelWork(ParallelWorker *worker, int units);

    /**
     * Create a parallel work handler.
     *
     * This is a compatibility constructor for older code, use the constructor with ParallelWorker instead.
     */
    ParallelWork(ParallelUnitFunction func, int units, void *data);

    /**
     * Delete a parallel work handler.
     *
     * The work must be terminated or fully interrupted before calling this.
     */
    ~ParallelWork();

    /**
     * Start working on the units.
     *
     * @param threads Number of threads to spaws, -1 for an optimal number.
     */
    int perform(int thread_count = -1);

    /**
     * Tell the threads to interrupt what they are doing.
     *
     * This will also call interrupt() on the worker.
     */
    void interrupt();

  private:
    void returnThread(ParallelThread *thread);

  private:
    int units;
    int running;
    ParallelWorker *worker;
    bool worker_compat;

    int thread_count;
    Mutex *mutex;
    Semaphore *semaphore;
    ParallelThread **threads;
    ParallelThread **available;
    int available_offset;
    int available_length;
};
}
}

#endif // PARALLELWORK_H
