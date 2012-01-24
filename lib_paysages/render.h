#ifndef _PAYSAGES_RENDER_H_
#define _PAYSAGES_RENDER_H_

#include <stdio.h>
#include "shared/types.h"
#include "renderer.h"

#ifdef __cplusplus
extern "C" {
#endif

void renderInit();
void renderSave(FILE* f);
void renderLoad(FILE* f);
void renderSetSize(int width, int height);
int renderSetQuality(int quality);
void renderClear();
void renderUpdate();
void renderInterrupt();
void renderSetBackgroundColor(Color* col);
void renderAddFragment(RenderFragment* fragment);
void renderPushFragment(int x, int y, double z, Vertex* vertex);
void renderPushTriangle(Renderer* renderer, Vertex* v1, Vertex* v2, Vertex* v3);
void renderPushQuad(Renderer* renderer, Vertex* v1, Vertex* v2, Vertex* v3, Vertex* v4);
void renderPostProcess(Renderer* renderer, int nbchunks);
void renderSaveToFile(const char* path);
void renderSetPreviewCallbacks(PreviewCallbackResize resize, PreviewCallbackClear clear, PreviewCallbackDraw draw, PreviewCallbackUpdate update);
int renderSetNextProgressStep(double start, double end);
int renderTellProgress(double progress);


#ifdef __cplusplus
}
#endif

#endif
