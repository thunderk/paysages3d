#ifndef _PAYSAGES_RENDER_H_
#define _PAYSAGES_RENDER_H_

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

void renderInit();
void renderQuit();

RenderArea* renderCreateArea();
void renderDeleteArea(RenderArea* area);

void renderSetParams(RenderArea* area, RenderParams params);
void renderSetToneMapping(RenderArea* area, ToneMappingOperator tonemapper, double exposure);
void renderSetBackgroundColor(RenderArea* area, Color* col);
void renderClear(RenderArea* area);
void renderUpdate(RenderArea* area);

void renderPushTriangle(RenderArea* area, Vector3 pixel1, Vector3 pixel2, Vector3 pixel3, Vector3 location1, Vector3 location2, Vector3 location3, f_RenderFragmentCallback callback, void* callback_data);

void renderPostProcess(RenderArea* area, Renderer* renderer, int nbchunks);
int renderSaveToFile(RenderArea* area, const char* path);
void renderSetPreviewCallbacks(RenderArea* area, RenderCallbackStart start, RenderCallbackDraw draw, RenderCallbackUpdate update);


#ifdef __cplusplus
}
#endif

#endif
