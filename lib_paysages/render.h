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

void renderAddFragment(RenderArea* area, RenderFragment* fragment);
void renderPushFragment(RenderArea* area, int x, int y, float z, Vertex* vertex);
void renderPushTriangle(RenderArea* area, Vertex* v1, Vertex* v2, Vertex* v3, Vector3 p1, Vector3 p2, Vector3 p3);

void renderPostProcess(RenderArea* area, Renderer* renderer, int nbchunks);
int renderSaveToFile(RenderArea* area, const char* path);
void renderSetPreviewCallbacks(RenderArea* area, RenderCallbackStart start, RenderCallbackDraw draw, RenderCallbackUpdate update);


#ifdef __cplusplus
}
#endif

#endif
