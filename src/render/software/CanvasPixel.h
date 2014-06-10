#ifndef CANVASPIXEL_H
#define CANVASPIXEL_H

#include "software_global.h"

#include "CanvasFragment.h"

const int MAX_FRAGMENT_COUNT = 7;

namespace paysages {
namespace software {

/**
 * @brief One pixel of a Canvas.
 *
 * A pixel stores superimposed fragments (CanvasFragment), sorted by their distance to camera.
 */
class SOFTWARESHARED_EXPORT CanvasPixel
{
public:
    CanvasPixel();

    inline int getFragmentCount() const {return count;}
    const CanvasFragment *getFrontFragment() const;

    void reset();
    void pushFragment(const CanvasFragment &fragment);

private:
    int count;
    CanvasFragment fragments[MAX_FRAGMENT_COUNT];
};

}
}

#endif // CANVASPIXEL_H
