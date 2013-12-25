#ifndef PARALLELPOOL_H
#define PARALLELPOOL_H

#include "system_global.h"

namespace paysages {
namespace system {

/*!
 * Pool to handle a group of threads doing the same task.
 */
class ParallelPool
{
public:
    ParallelPool();
    virtual ~ParallelPool();

    /*!
     * Start the effective work.
     */
    void start(int thread_count=-1);

    /*!
     * Method called from each thread to do actual work.
     */
    virtual void work() = 0;

    /*!
     * Method called once to interrupt all threads.
     */
    virtual void interrupt();

protected:
    bool running;

private:
    std::vector<Thread*> threads;
};

}
}

#endif // PARALLELPOOL_H
