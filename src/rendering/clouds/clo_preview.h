#ifndef _PAYSAGES_CLOUDS_PREVIEW_H_
#define _PAYSAGES_CLOUDS_PREVIEW_H_

#include "public.h"
#include "../tools/euclid.h"

/**
 * Cloud preview helpers.
 */

#ifdef __cplusplus
extern "C"
{
#endif

RENDERINGSHARED_EXPORT Renderer* cloudsPreviewCoverageCreateRenderer();
RENDERINGSHARED_EXPORT void cloudsPreviewCoverageBindLayer(Renderer* renderer, CloudsLayerDefinition* layer);
RENDERINGSHARED_EXPORT Color cloudsPreviewCoverageGetPixel(Renderer* renderer, double x, double y, double scaling, int perspective);

RENDERINGSHARED_EXPORT Renderer* cloudsPreviewMaterialCreateRenderer();
RENDERINGSHARED_EXPORT void cloudsPreviewMaterialBindLayer(Renderer* renderer, CloudsLayerDefinition* layer);
RENDERINGSHARED_EXPORT Color cloudsPreviewMaterialGetPixel(Renderer* renderer, double x, double y);

#ifdef __cplusplus
}
#endif

#endif
