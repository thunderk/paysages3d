#ifndef _PAYSAGES_TOOLS_PARALLEL_H_
#define _PAYSAGES_TOOLS_PARALLEL_H_

#include "../rendering_global.h"

/*
 * Parallel processing helpers.
 *
 * Several units of work can be accomplished by a given number of parallel workers.
 * Workers are implemented by threads so thread-safety must be ensured while accessing
 * shared data from unit functions.
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ParallelWork ParallelWork;
typedef int (*ParallelUnitFunction)(ParallelWork* work, int unit, void* data);

/**
 * Create a parallel work handler.
 *
 * This will spawn an optimal number of threads to process a given number of work units.
 * @param func The callback that will be called from threads to process one unit.
 * @param units Number of units to handle.
 * @param data Custom data that will be passed to the callback.
 * @return The newly allocated handler.
 */
RENDERINGSHARED_EXPORT ParallelWork* parallelWorkCreate(ParallelUnitFunction func, int units, void* data);

/**
 * Delete a parallel work handler.
 *
 * The work must be terminated or fully interrupted before calling this.
 * @param work The handler to free.
 */
RENDERINGSHARED_EXPORT void parallelWorkDelete(ParallelWork* work);

/**
 * Start working on the units.
 *
 * @param work The handler.
 * @param workers Number of threads to spaws, -1 for an optimal number.
 */
RENDERINGSHARED_EXPORT int parallelWorkPerform(ParallelWork* work, int workers);



typedef struct ParallelQueue ParallelQueue;
typedef int (*FuncParallelJob)(ParallelQueue* queue, int job_id, void* data, int stopping);

/**
 * Create a parallel processing queue.
 *
 * This queue will use parallel workers to process jobs added to it.
 * @param collect True to collect finished jobs and wait for a call to parallelQueueCollectJobs, False to discard finished jobs.
 * @return The newly allocated queue.
 */
RENDERINGSHARED_EXPORT ParallelQueue* parallelQueueCreate(int collect);

/**
 * Delete a parallel queue.
 *
 * This will interrupt the queue.
 * If the queue is in collect mode, you should call parallelQueueInterrupt, then parallelQueueCollectJobs, before calling this.
 * @param queue The queue to free.
 */
RENDERINGSHARED_EXPORT void parallelQueueDelete(ParallelQueue* queue);

/**
 * Interrupt the queue processing.
 *
 * This will wait for running jobs to end, cancel pending jobs (still calling their callbacks with stopping=1) and
 * refuse future jobs.
 * @param queue The queue to interrupt.
 */
RENDERINGSHARED_EXPORT void parallelQueueInterrupt(ParallelQueue* queue);

/**
 * Wait for all jobs to finish.
 *
 * This function will return as soon as there is no pending jobs. It is recommended to stop feeding the queue, or this
 * function may never return.
 */
RENDERINGSHARED_EXPORT void parallelQueueWait(ParallelQueue* queue);

/**
 * Add a job to the queue.
 *
 * Don't call this method concurrently from several threads.
 * @param queue The queue.
 * @param func_process The function that will be called for the job processing.
 * @param data The data that will be passed to the callback.
 * @return The job ID, 0 if the queue doesn't accept jobs.
 */
RENDERINGSHARED_EXPORT int parallelQueueAddJob(ParallelQueue* queue, FuncParallelJob func_process, void* data);

/**
 * Collect finished jobs.
 *
 * The callback func_collect will be called sequentially for each finished job, from the caller thread (not parallel threads).
 * Don't call this method concurrently from several threads.
 * @param func_collect The callback for collect.
 * @return The number of collected jobs.
 */
RENDERINGSHARED_EXPORT int parallelQueueCollectJobs(FuncParallelJob func_collect);

#ifdef __cplusplus
}
#endif

#endif
