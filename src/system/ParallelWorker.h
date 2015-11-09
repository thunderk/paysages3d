#ifndef PARALLELWORKER_H
#define PARALLELWORKER_H

#include "system_global.h"

namespace paysages {
namespace system {

/**
 * @brief Worker that can be used by the ParallelWork object to perform tasks in several threads.
 */
class SYSTEMSHARED_EXPORT ParallelWorker {
  public:
    ParallelWorker();
    virtual ~ParallelWorker();

    /**
     * Abstract method to reimplement to process a work unit.
     *
     * This method will be called from any thread in the thread pool used by the ParallelWork.
     */
    virtual void processParallelUnit(int unit) = 0;

    /**
     * Method to reimplement to know when to interrupt the processing of units.
     */
    virtual void interrupt();

  protected:
    bool interrupted;
};
}
}

#endif // PARALLELWORKER_H
