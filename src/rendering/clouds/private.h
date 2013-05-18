#ifndef _PAYSAGES_CLOUDS_PRIVATE_H_
#define _PAYSAGES_CLOUDS_PRIVATE_H_

#include "public.h"

#define CLOUDS_MAX_LAYERS 6
#define MAX_SEGMENT_COUNT 100

void cloudsLayerValidateDefinition(CloudsLayerDefinition* definition);

Color cloudsLayerFilterLight(CloudsLayerDefinition* definition, Renderer* renderer, Color light, Vector3 location, Vector3 light_location, Vector3 direction_to_light);
Color cloudsApplyLayer(CloudsLayerDefinition* definition, Color base, Renderer* renderer, Vector3 start, Vector3 end);

#endif
