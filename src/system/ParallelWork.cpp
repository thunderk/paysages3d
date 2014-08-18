#include "ParallelWork.h"

#include "Thread.h"
#include "System.h"
#include "ParallelWorker.h"
#include <cassert>

/**
 * Compatibility class for code that uses ParallelUnitFunction.
 */
class ParallelWorkerCompat:public ParallelWorker
{
public:
    ParallelWorkerCompat(ParallelWork *work, ParallelWork::ParallelUnitFunction func, void* data):
        ParallelWorker(), work(work), func(func), data(data)
    {
    }

    virtual int processParallelUnit(int unit)
    {
        return func(work, unit, data);
    }

private:
    ParallelWork* work;
    ParallelWork::ParallelUnitFunction func;
    void* data;
};

ParallelWork::ParallelWork(ParallelWorker *worker, int units)
{
    this->units = units;
    this->running = 0;
    this->worker = worker;
    this->worker_compat = false;
}

ParallelWork::ParallelWork(ParallelUnitFunction func, int units, void* data)
{
    this->units = units;
    this->running = 0;
    this->worker = new ParallelWorkerCompat(this, func, data);
    this->worker_compat = true;
}

ParallelWork::~ParallelWork()
{
    assert(not running);
    if (worker_compat)
    {
        delete worker;
    }
}

static void* _workerThreadCallback(ParallelWork::ParallelWorkerThread* thread)
{
    thread->result = thread->worker->processParallelUnit(thread->unit);
    thread->status = ParallelWork::PARALLEL_WORKER_STATUS_DONE;
    return NULL;
}

static int _runNextWorker(ParallelWork::ParallelWorkerThread threads[], int thread_count, int unit)
{
    int i;

    while (1)
    {
        for (i = 0; i < thread_count; i++)
        {
            ParallelWork::ParallelWorkerThread* worker = threads + i;
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

int ParallelWork::perform(int thread_count)
{
    int i, done, result;
    assert(not running);

    result = 0;

    if (thread_count <= 0)
    {
        thread_count = System::getCoreCount();
    }
    if (thread_count > PARALLEL_MAX_THREADS)
    {
        thread_count = PARALLEL_MAX_THREADS;
    }
    running = 1;

    /* Init workers */
    for (i = 0; i < thread_count; i++)
    {
        threads[i].status = PARALLEL_WORKER_STATUS_VOID;
        threads[i].worker = worker;
    }

    /* Perform run */
    for (done = 0; done < units; done++)
    {
        if (_runNextWorker(threads, thread_count, done))
        {
            result++;
        }
    }

    /* Wait and clean up workers */
    for (i = 0; i < thread_count; i++)
    {
        if (threads[i].status != PARALLEL_WORKER_STATUS_VOID)
        {
            threads[i].thread->join();
            delete threads[i].thread;
            if (threads[i].result)
            {
                result++;
            }
        }
    }

    running = 0;
    return result;
}
