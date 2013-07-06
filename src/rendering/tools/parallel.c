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
        mutexAcquire(queue->lock);
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
        mutexRelease(queue->lock);

        if (job)
        {
            /* Process the job */
            job->process(queue, job->id, job->data, 0);

            mutexAcquire(queue->lock);
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
            mutexRelease(queue->lock);
        }
        else
        {
            timeSleepMs(50);
        }
    }
    return NULL;
}

ParallelQueue* parallelQueueCreate(int collect)
{
    int i;

    assert(!collect); /* Not fully implemented yet ! */

    ParallelQueue* queue = malloc(sizeof(ParallelQueue));

    queue->collect = collect;
    queue->stopping = 0;
    queue->lock = mutexCreate();

    queue->jobs = malloc(sizeof(ParallelJob) * QUEUE_SIZE);
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
    queue->workers_count = systemGetCoreCount();
    queue->workers = malloc(sizeof(Thread*) * queue->workers_count);
    for (i = 0; i < queue->workers_count; i++)
    {
        queue->workers[i] = threadCreate((ThreadFunction)_queueThreadCallback, queue);
    }

    return queue;
}

void parallelQueueDelete(ParallelQueue* queue)
{
    parallelQueueInterrupt(queue);

    assert(!queue->collect || queue->jobs[queue->jobs_index_collect].state != JOB_STATE_TOCOLLECT);
    assert(queue->jobs_count == 0);

    mutexDestroy(queue->lock);
    free(queue->jobs);
    free(queue->workers);
    free(queue);
}

void parallelQueueInterrupt(ParallelQueue* queue)
{
    int i;

    if (!queue->stopping)
    {
        queue->stopping = 1;

        for (i = 0; i < queue->workers_count; i++)
        {
            threadJoin(queue->workers[i]);
        }
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
        timeSleepMs(50);
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
    mutexAcquire(queue->lock);
    if (queue->stopping)
    {
        mutexRelease(queue->lock);
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
    mutexRelease(queue->lock);

    return job.id;
}

int parallelQueueCollectJobs(FuncParallelJob func_collect)
{
    /* TODO */
    return 0;
}
