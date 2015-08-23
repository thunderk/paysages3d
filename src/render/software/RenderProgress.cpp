#include "RenderProgress.h"

#include "Mutex.h"

RenderProgress::RenderProgress(int count)
{
    lock = new Mutex();
    global = 0.0;
    step = 1.0 / (double)count;
}

paysages::software::RenderProgress::~RenderProgress()
{
    delete lock;
}

void RenderProgress::add(int value)
{
    lock->acquire();

    global += step * (double)value;

    lock->release();
}

void RenderProgress::enterSub(int count)
{
    struct RenderSub sub;
    sub.start = global;
    sub.end = global + step;
    sub.count = count;
    sub.previous_step = step;

    lock->acquire();

    subs.push(sub);
    step /= (double)count;

    lock->release();
}

void RenderProgress::exitSub()
{
    lock->acquire();

    struct RenderSub sub = subs.top();

    global = sub.end;
    step = sub.previous_step;

    subs.pop();

    lock->release();
}
