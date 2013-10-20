#ifndef _PAYSAGES_SHARED_PREVIEW_H_
#define _PAYSAGES_SHARED_PREVIEW_H_

/*
 * Preview management.
 */

#include "../rendering_global.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PreviewRenderer PreviewRenderer;

typedef void (*FuncPreviewCustomizeRenderer)(PreviewRenderer* preview_renderer, Renderer* renderer);
typedef void (*FuncPreviewBindDefinition)(PreviewRenderer* renderer, void* definition);
typedef Color (*FuncPreviewGetPixelColor)(PreviewRenderer* renderer, double x, double y, double scaling);

typedef struct {
    PreviewRenderer* (*createRenderer)();
    void (*deleteRenderer)(PreviewRenderer* renderer);

    void (*setCamera)(PreviewRenderer* renderer, Vector3 location, Vector3 target);

    FuncPreviewCustomizeRenderer customizeRenderer;
    FuncPreviewBindDefinition bindDefinition;
    FuncPreviewGetPixelColor getPixelColor;
} PreviewClass;

RENDERINGSHARED_EXPORT PreviewClass previewCreateClass(FuncPreviewCustomizeRenderer customizeRenderer, FuncPreviewBindDefinition bindDefinition, FuncPreviewGetPixelColor getPixelColor);

#ifdef __cplusplus
}
#endif

#endif
