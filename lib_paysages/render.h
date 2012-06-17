#ifndef _PAYSAGES_RENDER_H_
#define _PAYSAGES_RENDER_H_

#include <stdio.h>
#include "shared/types.h"
#include "renderer.h"

#ifdef __cplusplus
extern "C" {
#endif
    
void renderInit();
void renderQuit();

RenderArea* renderCreateArea();
void renderDeleteArea(RenderArea* area);

void renderSetParams(RenderArea* area, RenderParams params);
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
