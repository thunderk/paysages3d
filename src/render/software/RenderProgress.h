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
class SOFTWARESHARED_EXPORT RenderProgress
{
public:
    RenderProgress(int count=1);
    ~RenderProgress();

    inline double get() const {return global;}

    void add(int value=1);
    void enterSub(int count);
    void exitSub();

private:
    Mutex *lock;
    double global;
    double step;
    std::stack<RenderSub> subs;
};

}
}

#endif // RENDERPROGRESS_H