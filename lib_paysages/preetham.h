#ifndef _PAYSAGES_PREETHAM_H_
#define _PAYSAGES_PREETHAM_H_

/* Implementation of Preetham/Shirley light scattering */

#include "color.h"
#include "euclid.h"

#ifdef __cplusplus
extern "C" {
#endif

Color preethamGetSkyColor(Vector3 viewer, Vector3 direction, Vector3 sun_direction, double turbidity);
Color preethamApplyToObject(Vector3 viewer, Vector3 object_location, Vector3 sun_direction, double turbidity, Color object_color);

#ifdef __cplusplus
}
#endif

#endif
