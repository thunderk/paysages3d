#include "public.h"

#ifndef _PAYSAGES_ATMOSPHERE_PRIVATE_H_
#define _PAYSAGES_ATMOSPHERE_PRIVATE_H_

#define SPHERE_SIZE 10000.0

/* Factor to convert software units to kilometers */
#define WORLD_SCALING 0.05
#define SUN_DISTANCE 149597870.0
#define SUN_DISTANCE_SCALED (SUN_DISTANCE / WORLD_SCALING)
#define SUN_RADIUS 6.955e5
#define SUN_RADIUS_SCALED (SUN_RADIUS / WORLD_SCALING)

void brunetonInit();
AtmosphereResult brunetonGetSkyColor(Renderer* renderer, Vector3 eye, Vector3 direction, Vector3 sun_position, Color base);
AtmosphereResult brunetonApplyAerialPerspective(Renderer* renderer, Vector3 location, Color base);
void brunetonGetLightingStatus(Renderer* renderer, LightStatus* status, Vector3 normal, int opaque);

#endif
