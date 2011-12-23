#ifndef _PAYSAGES_GLOBALS_H_
#define _PAYSAGES_GLOBALS_H_

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

extern Vector3 camera_location;

extern int render_width;
extern int render_height;
extern int render_quality;

extern double sun_color_lum;
extern Vector3 sun_direction;
extern Vector3 sun_direction_inv;

#ifdef __cplusplus
}
#endif

#endif
