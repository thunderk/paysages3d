#include "ParallelPool.h"

#include "System.h"
#include "Thread.h"

static void* _threadFunction(void* data)
{
    ParallelPool* pool = (ParallelPool*)data;
    pool->work();
    return NULL;
}

ParallelPool::ParallelPool()
{
    running = false;
}

ParallelPool::~ParallelPool()
{
    if (running)
    {
        interrupt();
    }
    for (auto thread : threads)
    {
        thread->join();
        delete thread;
    }
    threads.clear();
}

void ParallelPool::start(int thread_count)
{
    if (running)
    {
        qCritical("Starting an already started parallel pool !");
        return;
    }
    running = true;
    if (thread_count < 0)
    {
        thread_count = System::getCoreCount();
    }
    for (int i = 0; i < thread_count; i++)
    {
        Thread* thread = new Thread(_threadFunction);
        thread->start(this);
        threads.push_back(thread);
    }
}

void ParallelPool::interrupt()
{
    running = false;
}
