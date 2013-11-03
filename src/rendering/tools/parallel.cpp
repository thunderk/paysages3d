#include "parallel.h"

#include <assert.h>
#include <stdlib.h>
#include "System.h"
#include "Thread.h"
#include "Mutex.h"

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
                worker->thread = new Thread((ThreadFunction)_workerThreadCallback);
                worker->thread->start(worker);

                return 0;
            }
            else if (worker->status == PARALLEL_WORKER_STATUS_DONE)
            {
                int result = worker->result;

                worker->status = PARALLEL_WORKER_STATUS_RUNNING;
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

int parallelWorkPerform(ParallelWork* work, int workers)
{
    int i, done, result;
    assert(!work->running);

    result = 0;

    if (workers <= 0)
    {
        workers = System::getCoreCount();
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
            work->workers[i].thread->join();
            delete work->workers[i].thread;
            if (work->workers[i].result)
            {
                result++;
            }
        }
    }

    work->running = 0;
    return result;
}


#define QUEUE_SIZE 1000

typedef enum
{
    JOB_STATE_FREE,
    JOB_STATE_PENDING,
    JOB_STATE_PROCESSING,
    JOB_STATE_TOCOLLECT
} EnumJobState;

typedef struct
{
    EnumJobState state;
    int id;
    FuncParallelJob process;
    void* data;
} ParallelJob;

struct ParallelQueue
{
    int collect;
    volatile int stopping;
    Mutex* lock;

    int workers_count;
    Thread** workers;

    ParallelJob* jobs;
    int jobs_count;          /** Number of jobs in queue (all status except JOB_STATE_FREE) */
    int jobs_index_free;     /** Index of next free position */
    int jobs_index_collect;  /** Index of first job to collect */
    int jobs_index_pending;  /** Index of first pending job to process */
    int jobs_next_id;
};

static void* _queueThreadCallback(ParallelQueue* queue)
{
    ParallelJob* job;

    while (!queue->stopping)
    {
        /* Try to take a job */
        queue->lock->acquire();
        job = queue->jobs + queue->jobs_index_pending;
        if (job->state == JOB_STATE_PENDING)
        {
            if (queue->jobs_index_pending >= QUEUE_SIZE - 1)
            {
                queue->jobs_index_pending = 0;
            }
            else
            {
                queue->jobs_index_pending++;
            }
            job->state = JOB_STATE_PROCESSING;
        }
        else
        {
            job = NULL;
        }
        queue->lock->release();

        if (job)
        {
            /* Process the job */
            job->process(queue, job->id, job->data, 0);

            queue->lock->acquire();
            if (queue->collect)
            {
                job->state = JOB_STATE_TOCOLLECT;
                /* TODO jobs_index_collect ? */
            }
            else
            {
                job->state = JOB_STATE_FREE;
                queue->jobs_count--;
            }
            queue->lock->release();
        }
        else
        {
            Thread::timeSleepMs(50);
        }
    }
    return NULL;
}

ParallelQueue* parallelQueueCreate(int collect)
{
    int i;

    assert(!collect); /* Not fully implemented yet ! */

    ParallelQueue* queue = new ParallelQueue;

    queue->collect = collect;
    queue->stopping = 0;
    queue->lock = new Mutex();

    queue->jobs = new ParallelJob[QUEUE_SIZE];
    for (i = 0; i < QUEUE_SIZE; i++)
    {
        queue->jobs[i].state = JOB_STATE_FREE;
    }
    queue->jobs_count = 0;
    queue->jobs_index_free = 0;
    queue->jobs_index_collect = 0;
    queue->jobs_index_pending = 0;
    queue->jobs_next_id = 1;

    /* Start workers */
    queue->workers_count = System::getCoreCount();
    queue->workers = new Thread*[queue->workers_count];
    for (i = 0; i < queue->workers_count; i++)
    {
        queue->workers[i] = new Thread((ThreadFunction)_queueThreadCallback);
        queue->workers[i]->start(queue);
    }

    return queue;
}

void parallelQueueDelete(ParallelQueue* queue)
{
    parallelQueueInterrupt(queue);

    assert(!queue->collect || queue->jobs[queue->jobs_index_collect].state != JOB_STATE_TOCOLLECT);
    assert(queue->jobs_count == 0);

    delete queue->lock;
    delete[] queue->jobs;
    delete[] queue->workers;
    delete queue;
}

void parallelQueueInterrupt(ParallelQueue* queue)
{
    int i;

    if (!queue->stopping)
    {
        queue->stopping = 1;

        for (i = 0; i < queue->workers_count; i++)
        {
            queue->workers[i]->join();
            delete queue->workers[i];
        }
    }
}

void parallelQueueWait(ParallelQueue* queue)
{
    while (queue->jobs_count > 0)
    {
        Thread::timeSleepMs(100);
    }
}

int parallelQueueAddJob(ParallelQueue* queue, FuncParallelJob func_process, void* data)
{
    if (queue->stopping)
    {
        return 0;
    }

    /* Wait for a free slot */
    while (queue->jobs[queue->jobs_index_free].state != JOB_STATE_FREE)
    {
        Thread::timeSleepMs(50);
        if (queue->stopping)
        {
            return 0;
        }
    }

    /* Prepare the job */
    ParallelJob job;
    job.state = JOB_STATE_PENDING;
    job.id = queue->jobs_next_id++;
    job.process = func_process;
    job.data = data;

    /* Add the job to the queue */
    queue->lock->acquire();
    if (queue->stopping)
    {
        queue->lock->release();
        return 0;
    }
    queue->jobs[queue->jobs_index_free] = job;
    if (queue->jobs_index_free >= QUEUE_SIZE - 1)
    {
        queue->jobs_index_free = 0;
    }
    else
    {
        queue->jobs_index_free++;
    }
    queue->jobs_count++;
    assert(queue->jobs_count <= QUEUE_SIZE);
    queue->lock->release();

    return job.id;
}

int parallelQueueCollectJobs(FuncParallelJob)
{
    /* TODO */
    return 0;
}