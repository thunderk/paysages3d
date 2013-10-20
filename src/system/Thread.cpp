#include "Thread.h"

Thread::Thread(ThreadFunction function):
    function(function), data(0), result(0)
{
}

void Thread::start(void* data)
{
    this->data = data;
    QThread::start();
}

void* Thread::join()
{
    QThread::wait();
    return result;
}

void Thread::run()
{
    result = function(data);
}

// Transitional C-API

Thread* threadCreate(ThreadFunction function, void* data)
{
    Thread* result = new Thread(function);

    result->start(data);

    return result;
}

void* threadJoin(Thread* thread)
{
    void* result;

    result = thread->join();

    delete thread;

    return result;
}
