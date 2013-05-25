#ifndef _PAYSAGES_CLOUDS_LIGHTING_H_
#define _PAYSAGES_CLOUDS_LIGHTING_H_

#include "public.h"
#include "../tools/euclid.h"

/**
 * Cloud lighting helpers.
 */

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * Bind fake lighting functions to a renderer.
 */
void cloudsBindFakeLightingToRenderer(CloudsRenderer* renderer);

/*
 * Bind real lighting functions to a renderer.
 */
void cloudsBindRealLightingToRenderer(CloudsRenderer* renderer);

#ifdef __cplusplus
}
#endif

#endif
