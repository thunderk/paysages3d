#ifndef _RENDERING_TOOLS_ZONE_H_
#define _RENDERING_TOOLS_ZONE_H_

/**
 * Definition of a geographic area.
 */

#include "rendering/tools/euclid.h"
#include "rendering/tools/curve.h"
#include "rendering/tools/pack.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Zone Zone;

Zone* zoneCreate();
void zoneDelete(Zone* zone);
void zoneSave(PackStream* stream, Zone* zone);
void zoneLoad(PackStream* stream, Zone* zone);
void zoneCopy(Zone* source, Zone* destination);
void zoneClear(Zone* zone);

void zoneIncludeCircleArea(Zone* zone, double value, double centerx, double centerz, double softradius, double hardradius);
void zoneExcludeCircleArea(Zone* zone, double centerx, double centerz, double softradius, double hardradius);

void zoneGetHeightCurve(Zone* zone, Curve* curve);
void zoneSetHeightCurve(Zone* zone, Curve* curve);
void zoneAddHeightRangeQuick(Zone* zone, double value, double hardmin, double softmin, double softmax, double hardmax);

void zoneGetSlopeCurve(Zone* zone, Curve* curve);
void zoneSetSlopeCurve(Zone* zone, Curve* curve);
void zoneAddSlopeRangeQuick(Zone* zone, double value, double hardmin, double softmin, double softmax, double hardmax);

double zoneGetValue(Zone* zone, Vector3 location, Vector3 normal);

#ifdef __cplusplus
}
#endif

#endif
