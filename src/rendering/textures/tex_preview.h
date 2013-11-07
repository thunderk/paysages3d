#ifndef TEX_PREVIEW_H
#define	TEX_PREVIEW_H

#include "rendering/renderer.h"
#include "rendering/textures/public.h"

/* Single layer coverage */
RENDERINGSHARED_EXPORT void TexturesPreviewLayerCoverage_bind(Renderer* renderer, TexturesDefinition* definition);
RENDERINGSHARED_EXPORT Color TexturesPreviewLayerCoverage_getColor(Renderer* renderer, double x, double y, double scaling, int layer);

/* Single layer look */
RENDERINGSHARED_EXPORT void TexturesPreviewLayerLook_bind(Renderer* renderer, TexturesDefinition* definition);
RENDERINGSHARED_EXPORT Color TexturesPreviewLayerLook_getColor(Renderer* renderer, double x, double y, double scaling, int layer);

/* Cumulative color preview */
RENDERINGSHARED_EXPORT void TexturesPreviewCumul_bind(Renderer* renderer, TexturesDefinition* definition);
RENDERINGSHARED_EXPORT Color TexturesPreviewCumul_getColor(Renderer* renderer, double x, double y, double scaling, int layer);

#endif
