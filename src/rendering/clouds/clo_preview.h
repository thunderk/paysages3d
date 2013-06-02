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

Renderer* cloudsPreviewCoverageCreateRenderer();
void cloudsPreviewCoverageBindLayer(Renderer* renderer, CloudsLayerDefinition* layer);
Color cloudsPreviewCoverageGetPixel(Renderer* renderer, double x, double y, double scaling, int perspective);

Renderer* cloudsPreviewMaterialCreateRenderer();
void cloudsPreviewMaterialBindLayer(Renderer* renderer, CloudsLayerDefinition* layer);
Color cloudsPreviewMaterialGetPixel(Renderer* renderer, double x, double y);

#ifdef __cplusplus
}
#endif

#endif
