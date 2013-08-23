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

/* Single layer look */
void TexturesPreviewLayerLook_bind(Renderer* renderer, TexturesDefinition* definition);
Color TexturesPreviewLayerLook_getColor(Renderer* renderer, double x, double y, double scaling, int layer);

/* Cumulative color preview */
void TexturesPreviewCumul_bind(Renderer* renderer, TexturesDefinition* definition);
Color TexturesPreviewCumul_getColor(Renderer* renderer, double x, double y, double scaling, int layer);

#ifdef	__cplusplus
}
#endif

#endif
