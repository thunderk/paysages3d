#ifndef PARALLELQUEUE_H
#define PARALLELQUEUE_H

#include "system_global.h"

namespace paysages {
namespace system {

class SYSTEMSHARED_EXPORT ParallelQueue
{
public:
    typedef int (*FuncParallelJob)(ParallelQueue* queue, int job_id, void* data, int stopping);

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

public:
    /**
     * Create a parallel processing queue.
     *
     * This queue will use parallel workers to process jobs added to it.
     * @param collect True to collect finished jobs and wait for a call to collectJobs, False to discard finished jobs.
     * @return The newly allocated queue.
     */
    ParallelQueue(int collect=0);

    /**
     * Delete a parallel queue.
     *
     * This will interrupt the queue.
     * If the queue is in collect mode, you should call interrupt, then collectJobs, before calling this.
     */
    ~ParallelQueue();

    /**
     * Interrupt the queue processing.
     *
     * This will wait for running jobs to end, cancel pending jobs (still calling their callbacks with stopping=1) and
     * refuse future jobs.
     */
    void interrupt();

    /**
     * Wait for all jobs to finish.
     *
     * This function will return as soon as there is no pending jobs. It is recommended to stop feeding the queue, or this
     * function may never return.
     */
    void wait();

    /**
     * Add a job to the queue.
     *
     * Don't call this method concurrently from several threads.
     * @param func_process The function that will be called for the job processing.
     * @param data The data that will be passed to the callback.
     * @return The job ID, 0 if the queue doesn't accept jobs.
     */
    int addJob(FuncParallelJob func_process, void* data);

    int collectJobs(FuncParallelJob func_collect);

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

}
}

#endif // PARALLELQUEUE_H
