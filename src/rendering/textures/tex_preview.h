#ifndef TEX_PREVIEW_H
#define	TEX_PREVIEW_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "rendering/renderer.h"
#include "rendering/textures/public.h"

/* Single layer coverage */
void TexturesPreviewLayerCoverage_bind(Renderer* renderer, TexturesDefinition* definition);
Color TexturesPreviewLayerCoverage_getColor(Renderer* renderer, double x, double y, double scaling, int layer);

#ifdef	__cplusplus
}
#endif

#endif
