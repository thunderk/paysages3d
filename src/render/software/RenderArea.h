#ifndef RENDERAREA_H
#define RENDERAREA_H

#include "software_global.h"

#include "Color.h"

typedef struct RenderFragment RenderFragment;
typedef struct FragmentCallback FragmentCallback;

namespace paysages {
namespace software {

class SOFTWARESHARED_EXPORT RenderArea
{
public:
    typedef struct
    {
        int width;
        int height;
        int antialias;
        int quality;
    } RenderParams;

    typedef Color (*f_RenderFragmentCallback)(SoftwareRenderer* renderer, const Vector3 &location, void* data);
    typedef void (*RenderCallbackStart)(int width, int height, const Color &background);
    typedef void (*RenderCallbackDraw)(int x, int y, const Color &col);
    typedef void (*RenderCallbackUpdate)(double progress);

public:
    RenderArea(SoftwareRenderer* parent);
    ~RenderArea();

    void setParams(RenderParams params);
    void setToneMapping(const ColorProfile &profile);
    void setBackgroundColor(const Color& col);
    void clear();
    void update();

    void pushTriangle(const Vector3 &pixel1, const Vector3 &pixel2, const Vector3 &pixel3, const Vector3 &location1, const Vector3 &location2, const Vector3 &location3, f_RenderFragmentCallback callback, void* callback_data);

    Color getPixel(int x, int y);

    void postProcess(int nbchunks);
    int saveToFile(const std::string &path);
    void setPreviewCallbacks(RenderCallbackStart start, RenderCallbackDraw draw, RenderCallbackUpdate update);

    void setAllDirty();
    void processDirtyPixels();
    void pushFragment(int x, int y, double z, int edge, const Vector3 &location, int callback);

public:
    ColorProfile* hdr_mapping;
    SoftwareRenderer* renderer;
    RenderParams params;
    int pixel_count;
    int pixel_done;
    RenderFragment* pixels;
    int fragment_callbacks_count;
    FragmentCallback* fragment_callbacks;
    Color background_color;
    volatile int dirty_left;
    volatile int dirty_right;
    volatile int dirty_up;
    volatile int dirty_down;
    volatile int dirty_count;
    Mutex* lock;
    RenderCallbackStart callback_start;
    RenderCallbackDraw callback_draw;
    RenderCallbackUpdate callback_update;
};

}
}

#endif // RENDERAREA_H
