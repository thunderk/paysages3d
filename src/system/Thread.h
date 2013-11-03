#ifndef THREAD_H
#define THREAD_H

#include "system_global.h"
#include <QThread>

namespace paysages
{
namespace system
{

typedef void* (*ThreadFunction)(void* data);

/*!
 * \brief System thread
 */
class SYSTEMSHARED_EXPORT Thread: private QThread
{
public:
    /*!
     * \brief Create a new thread
     *
     * The thread is not started automatically. A call to method start() needs to be done.
     * \param function Function to call inside the thread once it is started
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

    static inline void timeSleepMs(unsigned long ms){ QThread::msleep(ms); }

protected:
    virtual void run();

private:
    ThreadFunction function;
    void* data;
    void* result;
};

}
}

#endif // THREAD_H
