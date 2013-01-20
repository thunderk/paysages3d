#include "public.h"

#ifndef _PAYSAGES_ATMOSPHERE_PRIVATE_H_
#define _PAYSAGES_ATMOSPHERE_PRIVATE_H_

#define SPHERE_SIZE 10000.0

Color basicApplyAerialPerspective(Renderer* renderer, Vector3 location, Color base);
void basicGetLightingStatus(Renderer* renderer, LightStatus* status, Vector3 normal, int opaque);

void brunetonInit();
Color brunetonGetSkyColor(AtmosphereDefinition* definition, Vector3 eye, Vector3 direction, Vector3 sun_position);
Color brunetonApplyAerialPerspective(Renderer* renderer, Vector3 location, Color base);
void brunetonGetLightingStatus(Renderer* renderer, LightStatus* status, Vector3 normal, int opaque);

Color preethamGetSkyColor(AtmosphereDefinition* definition, Vector3 eye, Vector3 direction, Vector3 sun_position);

#endif
