#ifndef _PAYSAGES_RENDER_H_
#define _PAYSAGES_RENDER_H_

#include "rendering_global.h"
#include "shared/types.h"
#include "tools/color.h"
#include "tools/euclid.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef Color (*f_RenderFragmentCallback)(struct Renderer* renderer, Vector3 location, void* data);

typedef void (*RenderCallbackStart)(int width, int height, Color background);
typedef void (*RenderCallbackDraw)(int x, int y, Color col);
typedef void (*RenderCallbackUpdate)(double progress);

typedef struct RenderArea RenderArea;

typedef struct
{
    int width;
    int height;
    int antialias;
    int quality;
} RenderParams;

RENDERINGSHARED_EXPORT void renderInit();
RENDERINGSHARED_EXPORT void renderQuit();

RENDERINGSHARED_EXPORT RenderArea* renderCreateArea(Renderer* renderer);
RENDERINGSHARED_EXPORT void renderDeleteArea(RenderArea* area);

RENDERINGSHARED_EXPORT void renderSetParams(RenderArea* area, RenderParams params);
RENDERINGSHARED_EXPORT void renderSetToneMapping(RenderArea* area, ToneMappingOperator tonemapper, double exposure);
RENDERINGSHARED_EXPORT void renderSetBackgroundColor(RenderArea* area, Color* col);
RENDERINGSHARED_EXPORT void renderClear(RenderArea* area);
RENDERINGSHARED_EXPORT void renderUpdate(RenderArea* area);

RENDERINGSHARED_EXPORT void renderPushTriangle(RenderArea* area, Vector3 pixel1, Vector3 pixel2, Vector3 pixel3, Vector3 location1, Vector3 location2, Vector3 location3, f_RenderFragmentCallback callback, void* callback_data);

RENDERINGSHARED_EXPORT Color renderGetPixel(RenderArea* area, int x, int y);

RENDERINGSHARED_EXPORT void renderPostProcess(RenderArea* area, int nbchunks);
RENDERINGSHARED_EXPORT int renderSaveToFile(RenderArea* area, const char* path);
RENDERINGSHARED_EXPORT void renderSetPreviewCallbacks(RenderArea* area, RenderCallbackStart start, RenderCallbackDraw draw, RenderCallbackUpdate update);


#ifdef __cplusplus
}
#endif

#endif
