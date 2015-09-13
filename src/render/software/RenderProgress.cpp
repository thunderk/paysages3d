#include "RenderProgress.h"

#include "Mutex.h"
#include "Time.h"
#include "Logs.h"

RenderProgress::RenderProgress(int count)
{
    lock = new Mutex();
    global = 0.0;
    step = 1.0 / (double)count;
    start_time = Time::getRelativeTimeMs();
    end_time = 0;
}

paysages::software::RenderProgress::~RenderProgress()
{
    delete lock;
}

void RenderProgress::reset()
{
    lock->acquire();

    global = 0.0;
    while (not subs.empty())
    {
        subs.pop();
    }

    start_time = Time::getRelativeTimeMs();

    lock->release();
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

void RenderProgress::end()
{
    if (subs.size() > 0)
    {
        Logs::error() << subs.size() << " progress subs remaining at the end of render" << std::endl;
    }

    end_time = Time::getRelativeTimeMs();
}

unsigned long RenderProgress::getDuration() const
{
    if (end_time)
    {
        return end_time - start_time;
    }
    else
    {
        return Time::getRelativeTimeMs() - start_time;
    }
}

unsigned long RenderProgress::estimateRemainingTime() const
{
    if (global < 0.00001)
    {
        return 0;
    }
    else
    {
        unsigned long done = getDuration();
        unsigned long total = (unsigned long)((double)done / global);
        return total - done;
    }
}
