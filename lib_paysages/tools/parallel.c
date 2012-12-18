#include "parallel.h"

#include <assert.h>
#include "../system.h"

#define PARALLEL_MAX_THREADS 20

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

struct ParallelWork
{
    int units;
    int running;
    ParallelUnitFunction unit_function;
    ParallelWorker workers[PARALLEL_MAX_THREADS];
    void* data;
};

ParallelWork* parallelWorkCreate(ParallelUnitFunction func, int units, void* data)
{
    ParallelWork* result;

    result = (ParallelWork*)malloc(sizeof(ParallelWork));
    result->units = units;
    result->running = 0;
    result->unit_function = func;
    result->data = data;

    return result;
}

void parallelWorkDelete(ParallelWork* work)
{
    assert(!work->running);
    free(work);
}

static void* _workerThreadCallback(ParallelWorker* worker)
{
    worker->result = worker->work->unit_function(worker->work, worker->unit, worker->work->data);
    worker->status = PARALLEL_WORKER_STATUS_DONE;
    return NULL;
}

static int _runNextWorker(ParallelWorker workers[], int worker_count, int unit)
{
    int i;

    while (1)
    {
        for (i = 0; i < worker_count; i++)
        {
            ParallelWorker* worker = workers + i;
            if (worker->status == PARALLEL_WORKER_STATUS_VOID)
            {
                worker->status = PARALLEL_WORKER_STATUS_RUNNING;
                worker->result = 0;
                worker->unit = unit;
                worker->thread = threadCreate((ThreadFunction)_workerThreadCallback, worker);

                return 0;
            }
            else if (worker->status == PARALLEL_WORKER_STATUS_DONE)
            {
                int result = worker->result;

                worker->status = PARALLEL_WORKER_STATUS_RUNNING;
                worker->result = 0;
                worker->unit = unit;
                threadJoin(worker->thread);
                worker->thread = threadCreate((ThreadFunction)_workerThreadCallback, worker);

                return result;
            }
        }
        timeSleepMs(50);
    }
}

int parallelWorkPerform(ParallelWork* work, int workers)
{
    int i, done, result;
    assert(!work->running);

    result = 0;

    if (workers <= 0)
    {
        workers = systemGetCoreCount();
    }
    if (workers > PARALLEL_MAX_THREADS)
    {
        workers = PARALLEL_MAX_THREADS;
    }
    work->running = 1;

    /* Init workers */
    for (i = 0; i < workers; i++)
    {
        work->workers[i].status = PARALLEL_WORKER_STATUS_VOID;
        work->workers[i].work = work;
    }

    /* Perform run */
    for (done = 0; done < work->units; done++)
    {
        if (_runNextWorker(work->workers, workers, done))
        {
            result++;
        }
    }

    /* Wait and clean up workers */
    for (i = 0; i < workers; i++)
    {
        if (work->workers[i].status != PARALLEL_WORKER_STATUS_VOID)
        {
            threadJoin(work->workers[i].thread);
            if (work->workers[i].result)
            {
                result++;
            }
        }
    }

    work->running = 0;
    return result;
}
