#ifndef _PAYSAGES_TOOLS_PARALLEL_H_
#define _PAYSAGES_TOOLS_PARALLEL_H_

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

/*void parallelInit();
void parallelQuit();*/

ParallelWork* parallelWorkCreate(ParallelUnitFunction func, int units, void* data);
void parallelWorkDelete(ParallelWork* work);
int parallelWorkPerform(ParallelWork* work, int workers);

#ifdef __cplusplus
}
#endif

#endif
