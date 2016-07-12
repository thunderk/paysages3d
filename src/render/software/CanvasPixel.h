#pragma once

#include "software_global.h"

#include "CanvasFragment.h"

const int MAX_FRAGMENTS_PER_PIXEL = 5;

namespace paysages {
namespace software {

/**
 * One pixel of a Canvas.
 *
 * A pixel stores superimposed fragments (CanvasFragment), sorted by their distance to camera.
 */
class SOFTWARESHARED_EXPORT CanvasPixel {
  public:
    CanvasPixel();

    inline int getFragmentCount() const {
        return count;
    }
    inline const Color &getComposite() const {
        return composite;
    }
    inline const CanvasFragment &getFragment(int position) const {
        return fragments[position];
    }
    const CanvasFragment *getFrontFragment() const;

    void reset();
    void pushFragment(const CanvasFragment &fragment);
    void updateComposite();
    void setComposite(const Color &color);

  private:
    int count;
    CanvasFragment fragments[MAX_FRAGMENTS_PER_PIXEL];
    Color composite;
};
}
}
