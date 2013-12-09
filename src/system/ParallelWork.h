#ifndef PARALLELWORK_H
#define PARALLELWORK_H

#include "system_global.h"

#define PARALLEL_MAX_THREADS 20

namespace paysages {
namespace system {

class SYSTEMSHARED_EXPORT ParallelWork
{
public:
    typedef int (*ParallelUnitFunction)(ParallelWork* work, int unit, void* data);

    typedef enum
    {
        PARALLEL_WORKER_STATUS_VOID,
        PARALLEL_WORKER_STATUS_RUNNING,
        PARALLEL_WORKER_STATUS_DONE
    } ParallelWorkerStatus;

    typedef struct
    {
        Thread* thread;
        ParallelWork* work;
        ParallelWorkerStatus status;
        int unit;
        int result;
    } ParallelWorker;

public:
    /**
     * Create a parallel work handler.
     *
     * This will spawn an optimal number of threads to process a given number of work units.
     *
     * @param func The callback that will be called from threads to process one unit.
     * @param units Number of units to handle.
     * @param data Custom data that will be passed to the callback.
     * @return The newly allocated handler.
     */
    ParallelWork(ParallelUnitFunction func, int units, void* data);

    /**
     * Delete a parallel work handler.
     *
     * The work must be terminated or fully interrupted before calling this.
     */
    ~ParallelWork();

    /**
     * Start working on the units.
     *
     * @param workers Number of threads to spaws, -1 for an optimal number.
     */
    int perform(int workers=-1);

    int units;
    int running;
    ParallelUnitFunction unit_function;
    ParallelWorker workers[PARALLEL_MAX_THREADS];
    void* data;
};

}
}

#endif // PARALLELWORK_H
