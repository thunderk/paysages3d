#include "ParallelWork.h"

#include "Thread.h"
#include "System.h"
#include <cassert>

ParallelWork::ParallelWork(ParallelUnitFunction func, int units, void* data)
{
    this->units = units;
    this->running = 0;
    this->unit_function = func;
    this->data = data;
}

ParallelWork::~ParallelWork()
{
    assert(not running);
}

static void* _workerThreadCallback(ParallelWork::ParallelWorker* worker)
{
    worker->result = worker->work->unit_function(worker->work, worker->unit, worker->work->data);
    worker->status = ParallelWork::PARALLEL_WORKER_STATUS_DONE;
    return NULL;
}

static int _runNextWorker(ParallelWork::ParallelWorker workers[], int worker_count, int unit)
{
    int i;

    while (1)
    {
        for (i = 0; i < worker_count; i++)
        {
            ParallelWork::ParallelWorker* worker = workers + i;
            if (worker->status == ParallelWork::PARALLEL_WORKER_STATUS_VOID)
            {
                worker->status = ParallelWork::PARALLEL_WORKER_STATUS_RUNNING;
                worker->result = 0;
                worker->unit = unit;
                worker->thread = new Thread((ThreadFunction)_workerThreadCallback);
                worker->thread->start(worker);

                return 0;
            }
            else if (worker->status == ParallelWork::PARALLEL_WORKER_STATUS_DONE)
            {
                int result = worker->result;

                worker->status = ParallelWork::PARALLEL_WORKER_STATUS_RUNNING;
                worker->result = 0;
                worker->unit = unit;
                worker->thread->join();
                delete worker->thread;
                worker->thread = new Thread((ThreadFunction)_workerThreadCallback);
                worker->thread->start(worker);

                return result;
            }
        }
        Thread::timeSleepMs(50);
    }
}

int ParallelWork::perform(int nbworkers)
{
    int i, done, result;
    assert(not running);

    result = 0;

    if (nbworkers <= 0)
    {
        nbworkers = System::getCoreCount();
    }
    if (nbworkers > PARALLEL_MAX_THREADS)
    {
        nbworkers = PARALLEL_MAX_THREADS;
    }
    running = 1;

    /* Init workers */
    for (i = 0; i < nbworkers; i++)
    {
        workers[i].status = PARALLEL_WORKER_STATUS_VOID;
        workers[i].work = this;
    }

    /* Perform run */
    for (done = 0; done < units; done++)
    {
        if (_runNextWorker(workers, nbworkers, done))
        {
            result++;
        }
    }

    /* Wait and clean up workers */
    for (i = 0; i < nbworkers; i++)
    {
        if (workers[i].status != PARALLEL_WORKER_STATUS_VOID)
        {
            workers[i].thread->join();
            delete workers[i].thread;
            if (workers[i].result)
            {
                result++;
            }
        }
    }

    running = 0;
    return result;
}
