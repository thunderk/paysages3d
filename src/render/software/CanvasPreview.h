#ifndef CANVASPREVIEW_H
#define CANVASPREVIEW_H

#include "software_global.h"

namespace paysages {
namespace software {

typedef struct {
    double red;
    double green;
    double blue;
} CanvasPreviewPixel;

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

    void initLive(CanvasLiveClient &client);
    void updateLive(CanvasLiveClient &client);

    void pushPixel(int real_x, int real_y, Color old_color, Color new_color);

private:
    CanvasPreviewPixel *pixels;
    int width;
    int height;
};

}
}

#endif // CANVASPREVIEW_H
