#ifndef CANVAS_H
#define CANVAS_H

#include "software_global.h"

namespace paysages {
namespace software {

/**
 * @brief Graphics area to draw and do compositing.
 *
 * Software rendering is done in portions of Canvas (in CanvasPortion class).
 * This splitting in portions allows to keep memory consumption low.
 */
class SOFTWARESHARED_EXPORT Canvas
{
public:
    Canvas();
    ~Canvas();

    void setSize(int width, int height);

    inline int getHorizontalPortionCount() const {return horizontal_portion_count;}
    inline int getVerticalPortionCount() const {return vertical_portion_count;}

    CanvasPortion *at(int x, int y) const;

    inline int getWidth() const {return width;}
    inline int getHeight() const {return height;}
    inline CanvasPreview *getPreview() const {return preview;}

private:
    std::vector<CanvasPortion*> portions;
    int horizontal_portion_count;
    int vertical_portion_count;
    int width;
    int height;

    CanvasPreview *preview;
};

}
}

#endif // CANVAS_H
