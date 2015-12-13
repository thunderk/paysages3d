#include "RenderProgress.h"

#include "Mutex.h"
#include "Time.h"
#include "Logs.h"

RenderProgress::RenderProgress(int count) {
    lock = new Mutex();
    global = 0.0;
    step = 1.0 / (double)count;
    start_time = 0;
    end_time = 0;
    reset();
}

paysages::software::RenderProgress::~RenderProgress() {
    delete lock;
}

void RenderProgress::reset() {
    lock->acquire();

    global = 0.0;
    while (not subs.empty()) {
        subs.pop();
    }

    start_time = Time::getRelativeTimeMs();

    prev_est_spent = 0;
    prev_est_done = 0.0;
    prev_est_speed = 0.0;

    lock->release();
}

void RenderProgress::add(int value) {
    lock->acquire();

    global += step * (double)value;

    lock->release();
}

void RenderProgress::enterSub(int count) {
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

void RenderProgress::exitSub() {
    lock->acquire();

    struct RenderSub sub = subs.top();

    global = sub.end;
    step = sub.previous_step;

    subs.pop();

    lock->release();
}

void RenderProgress::end() {
    if (subs.size() > 0) {
        Logs::error("Software") << subs.size() << " progress subs remaining at the end of render" << endl;
    }

    end_time = Time::getRelativeTimeMs();
}

unsigned long RenderProgress::getDuration() const {
    if (end_time) {
        return end_time - start_time;
    } else {
        return Time::getRelativeTimeMs() - start_time;
    }
}

unsigned long RenderProgress::estimateRemainingTime() {
    unsigned long spent = getDuration();
    double speed = (global - prev_est_done) / (double)(spent - prev_est_spent);

    prev_est_speed = prev_est_speed ? (prev_est_speed * 0.8 + speed * 0.2) : speed;
    if (spent - prev_est_spent > 5000) {
        prev_est_spent = spent;
        prev_est_done = global;
    }

    if (prev_est_speed > 0.0000000001) {
        return (unsigned long)((1.0 - global) / prev_est_speed);
    } else {
        return 0;
    }
}
