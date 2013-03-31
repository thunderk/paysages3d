#ifndef _PAYSAGES_TEXTURES_PRIVATE_H_
#define _PAYSAGES_TEXTURES_PRIVATE_H_

#include "public.h"

/*
 * Get the base presence factor of a layer, not accounting for other layers.
 */
double texturesGetLayerBasePresence(Renderer* renderer, TexturesLayerDefinition* layer, TerrainResult result);

/*
 * Get triplanar noise value, depending on the normal direction.
 */
double texturesGetTriplanarNoise(NoiseGenerator* noise, Vector3 location, Vector3 normal);

/*
 * Apply texture displacement on a terrain result.
 */
TerrainResult texturesApplyLayerDisplacement(Renderer* renderer, TexturesLayerDefinition* layer, TerrainResult initial, TerrainResult support, double presence, double cancel_factor);

#endif
