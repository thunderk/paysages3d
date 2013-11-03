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
