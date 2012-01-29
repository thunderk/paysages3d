#ifndef _PAYSAGES_ZONE_H_
#define _PAYSAGES_ZONE_H_

#include "shared/types.h"

#ifdef __cplusplus
extern "C" {
#endif

Zone* zoneCreate();
void zoneDelete(Zone* zone);
void zoneSave(FILE* f, Zone* zone);
void zoneLoad(FILE* f, Zone* zone);
void zoneCopy(Zone* source, Zone* destination);
void zoneIncludeCircleArea(Zone* zone, double value, double centerx, double centerz, double softradius, double hardradius);
void zoneExcludeCircleArea(Zone* zone, double centerx, double centerz, double softradius, double hardradius);
void zoneAddHeightRange(Zone* zone, double value, double hardmin, double softmin, double softmax, double hardmax);
void zoneAddSteepnessRange(Zone* zone, double value, double hardmin, double softmin, double softmax, double hardmax);
double zoneGetValue(Zone* zone, Vector3 location, Vector3 normal);

#ifdef __cplusplus
}
#endif

#endif
