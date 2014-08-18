#ifndef CANVASPREVIEW_H
#define CANVASPREVIEW_H

#include "software_global.h"

namespace paysages {
namespace software {

/**
 * @brief Smaller preview of a Canvas rendering, that can be watched live.
 */
class SOFTWARESHARED_EXPORT CanvasPreview
{
public:
    CanvasPreview();
    ~CanvasPreview();

    inline int getWidth() const {return width;}
    inline int getHeight() const {return height;}

    void setSize(int real_width, int real_height, int preview_width, int preview_height);
    void reset();

    void initLive(CanvasLiveClient *client);
    void updateLive(CanvasLiveClient *client);

    void pushPixel(int real_x, int real_y, const Color &old_color, const Color &new_color);

protected:
    void setAllDirty();

private:
    Mutex *lock;

    Color *pixels;
    int width;
    int height;

    ColorProfile *profile;

    int dirty_left;
    int dirty_right;
    int dirty_down;
    int dirty_up;
};

}
}

#endif // CANVASPREVIEW_H
