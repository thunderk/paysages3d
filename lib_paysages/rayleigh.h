#ifndef _PAYSAGES_RAYLEIGH_H_
#define _PAYSAGES_RAYLEIGH_H_

/* Implementation of Rayleigh/Mie atmospheric light scattering */

#include "color.h"
#include "euclid.h"

#ifdef __cplusplus
extern "C" {
#endif

Color rayleighGetSkyColor(Vector3 viewer, Vector3 direction, Vector3 sun_direction);
Color rayleighApplyToObject(Vector3 viewer, Vector3 object_location, Vector3 sun_direction, Color object_color);

#ifdef __cplusplus
}
#endif

#endif
