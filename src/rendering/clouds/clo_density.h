#ifndef _PAYSAGES_CLOUDS_DENSITY_H_
#define _PAYSAGES_CLOUDS_DENSITY_H_

#include "public.h"

/**
 * Coverage/density management in a cloud layer.
 */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Get the coverage of a cloud layer [0.0;1.0]
 *
 * 0.0 means no cloud is present.
 * 1.0 means full layer.
 */
double cloudsGetLayerCoverage(CloudsLayerDefinition* layer, Vector3 location);

/**
 * Get the global density of a cloud layer at a given point [0.0;1.0].
 *
 * 0.0 means no cloud is present.
 * 1.0 means full density (deep inside cloud).
 */
double cloudsGetLayerDensity(CloudsLayerDefinition* layer, Vector3 location, double coverage);

/**
 * Get the local density of a cloud layer at a given point inside an edge [0.0;1.0].
 */
double cloudsGetEdgeDensity(CloudsLayerDefinition* layer, Vector3 location, double layer_density);

/*
 * Bind fake density functions to a renderer.
 */
void cloudsBindFakeDensityToRenderer(CloudsRenderer* renderer);

/*
 * Bind real density functions to a renderer.
 */
void cloudsBindRealDensityToRenderer(CloudsRenderer* renderer);

#ifdef __cplusplus
}
#endif

#endif
