#include "Thread.h"

Thread::Thread(ThreadFunction function):
    data(0), result(0), function(function)
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
