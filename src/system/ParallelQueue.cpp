#include "ParallelQueue.h"

#include "Mutex.h"
#include "Thread.h"
#include "System.h"
#include <cassert>

#define QUEUE_SIZE 1000

static void* _queueThreadCallback(ParallelQueue* queue)
{
    ParallelQueue::ParallelJob* job;

    while (!queue->stopping)
    {
        /* Try to take a job */
        queue->lock->acquire();
        job = queue->jobs + queue->jobs_index_pending;
        if (job->state == ParallelQueue::JOB_STATE_PENDING)
        {
            if (queue->jobs_index_pending >= QUEUE_SIZE - 1)
            {
                queue->jobs_index_pending = 0;
            }
            else
            {
                queue->jobs_index_pending++;
            }
            job->state = ParallelQueue::JOB_STATE_PROCESSING;
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
                job->state = ParallelQueue::JOB_STATE_TOCOLLECT;
                /* TODO jobs_index_collect ? */
            }
            else
            {
                job->state = ParallelQueue::JOB_STATE_FREE;
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

ParallelQueue::ParallelQueue(int collect)
{
    int i;

    assert(!collect); /* Not fully implemented yet ! */

    this->collect = collect;
    this->stopping = 0;
    this->lock = new Mutex();

    this->jobs = new ParallelJob[QUEUE_SIZE];
    for (i = 0; i < QUEUE_SIZE; i++)
    {
        this->jobs[i].state = JOB_STATE_FREE;
    }
    this->jobs_count = 0;
    this->jobs_index_free = 0;
    this->jobs_index_collect = 0;
    this->jobs_index_pending = 0;
    this->jobs_next_id = 1;

    /* Start workers */
    this->workers_count = System::getCoreCount();
    this->workers = new Thread*[this->workers_count];
    for (i = 0; i < this->workers_count; i++)
    {
        this->workers[i] = new Thread((ThreadFunction)_queueThreadCallback);
        this->workers[i]->start(this);
    }
}

ParallelQueue::~ParallelQueue()
{
    interrupt();

    assert(not collect or jobs[jobs_index_collect].state != JOB_STATE_TOCOLLECT);
    assert(jobs_count == 0);

    delete lock;
    delete[] jobs;
    delete[] workers;
}

void ParallelQueue::interrupt()
{
    int i;

    if (not stopping)
    {
        stopping = 1;

        for (i = 0; i < workers_count; i++)
        {
            workers[i]->join();
            delete workers[i];
        }
    }
}

void ParallelQueue::wait()
{
    while (jobs_count > 0)
    {
        Thread::timeSleepMs(100);
    }
}

int ParallelQueue::addJob(FuncParallelJob func_process, void* data)
{
    if (stopping)
    {
        return 0;
    }

    /* Wait for a free slot */
    while (jobs[jobs_index_free].state != JOB_STATE_FREE)
    {
        Thread::timeSleepMs(50);
        if (stopping)
        {
            return 0;
        }
    }

    /* Prepare the job */
    ParallelJob job;
    job.state = JOB_STATE_PENDING;
    job.id = jobs_next_id++;
    job.process = func_process;
    job.data = data;

    /* Add the job to the queue */
    lock->acquire();
    if (stopping)
    {
        lock->release();
        return 0;
    }
    jobs[jobs_index_free] = job;
    if (jobs_index_free >= QUEUE_SIZE - 1)
    {
        jobs_index_free = 0;
    }
    else
    {
        jobs_index_free++;
    }
    jobs_count++;
    assert(jobs_count <= QUEUE_SIZE);
    lock->release();

    return job.id;
}

int ParallelQueue::collectJobs(FuncParallelJob)
{
    /* TODO */
    return 0;
}
