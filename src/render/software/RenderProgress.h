#ifndef RENDERPROGRESS_H
#define RENDERPROGRESS_H

#include "software_global.h"

#include <stack>

namespace paysages {
namespace software {

struct RenderSub {
    double start;
    double end;
    int count;
    double previous_step;
};

/**
 * Utility to keep track of render progress.
 */
class SOFTWARESHARED_EXPORT RenderProgress {
  public:
    RenderProgress(int count = 1);
    ~RenderProgress();

    inline double get() const {
        return global;
    }

    void reset();
    void add(int value = 1);
    void enterSub(int count);
    void exitSub();
    void end();

    /**
     * Get the render duration in milliseconds.
     */
    unsigned long getDuration() const;

    /**
     * Get the estimated remaining time in milliseconds.
     *
     * Returns 0 if unknown.
     */
    unsigned long estimateRemainingTime();

  private:
    Mutex *lock;

    double global;
    double step;

    unsigned long start_time;
    unsigned long end_time;

    unsigned long prev_est_spent;
    double prev_est_done;
    double prev_est_speed;

    std::stack<RenderSub> subs;
};
}
}

#endif // RENDERPROGRESS_H
