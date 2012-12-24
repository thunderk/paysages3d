#include "public.h"

#ifndef _PAYSAGES_ATMOSPHERE_PRIVATE_H_
#define _PAYSAGES_ATMOSPHERE_PRIVATE_H_

#define SPHERE_SIZE 1000.0

Color basicApplyAerialPerspective(Renderer* renderer, Vector3 location, Color base);

void brunetonInit();
Color brunetonGetSkyColor(AtmosphereDefinition* definition, Vector3 eye, Vector3 direction, Vector3 sun_position);
Color brunetonApplyAerialPerspective(Renderer* renderer, Vector3 location, Color base);

Color preethamGetSkyColor(AtmosphereDefinition* definition, Vector3 eye, Vector3 direction, Vector3 sun_position);

#endif
