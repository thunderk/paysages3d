#ifndef THREAD_H
#define THREAD_H

#include "system_global.h"
#include <QThread>

namespace paysages {
namespace system {

typedef void *(*ThreadFunction)(void *data);

/**
 * System thread
 */
class SYSTEMSHARED_EXPORT Thread : private QThread {
  public:
    /**
     * Create a new thread.
     *
     * The thread is not started automatically. A call to method start() needs to be done.
     *
     * Either the *function* argument should be provided (with the function to call from the
     * thread), or the *run* method overridden.
     */
    Thread(ThreadFunction function = 0);

    /**
     * Start the thread, with custom data.
     */
    void start(void *data = 0);

    /**
     * Wait for the thread to end, and collect its result.
     *
     * Returns the value returned by the threaded function.
     */
    void *join();

    /**
     * Return true if the thread is currently running.
     */
    inline bool isWorking() const {
        return not isFinished();
    }

    static inline void timeSleepMs(unsigned long ms) {
        QThread::msleep(ms);
    }

  protected:
    /**
     * Function to reimplement if no ThreadFunction has been passed to the constructor.
     */
    virtual void run();

    void *data;
    void *result;

  private:
    ThreadFunction function;
};
}
}

#endif // THREAD_H
