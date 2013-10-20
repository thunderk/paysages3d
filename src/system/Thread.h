#ifndef THREAD_H
#define THREAD_H

#include "system_global.h"

typedef void* (*ThreadFunction)(void* data);

#ifdef __cplusplus

#include <QThread>

namespace paysages
{
namespace system
{

/*!
 * \brief System thread
 */
class Thread: private QThread
{
public:
    /*!
     * \brief Create a new thread
     * \param function Function to call inside the thread once it is started
     *
     * The thread is not started automatically. A call to method start() needs to be done.
     */
    Thread(ThreadFunction function);

    /*!
     * \brief Start the thread
     * \param data User data to pass to the threaded function
     */
    void start(void* data);

    /*!
     * \brief Wait for the thread to end, and collect its result.
     * \return The value returned by the threaded function.
     */
    void* join();

protected:
    virtual void run();

private:
    ThreadFunction function;
    void* data;
    void* result;
};

}
}

extern "C" {
#endif

// Transitional C-API

#ifndef __cplusplus
typedef struct Thread Thread;
#endif

Thread* threadCreate(ThreadFunction function, void* data);
void* threadJoin(Thread* thread);

#ifdef __cplusplus
}
#endif

#endif // THREAD_H
