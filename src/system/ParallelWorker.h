#ifndef PARALLELWORKER_H
#define PARALLELWORKER_H

#include "system_global.h"

namespace paysages {
namespace system {

/**
 * @brief Worker that can be used by the ParallelWork object to perform tasks in several threads.
 */
class SYSTEMSHARED_EXPORT ParallelWorker
{
public:
    ParallelWorker();
    virtual ~ParallelWorker();

    /**
     * Abstract method to reimplement to process a work unit.
     *
     * This method will be called from any thread in the thread pool used by the ParallelWork.
     */
    virtual int processParallelUnit(int unit) = 0;
};

}
}

#endif // PARALLELWORKER_H
