#ifndef _PAYSAGES_ZONE_H_
#define _PAYSAGES_ZONE_H_

#include "shared/types.h"
#include "pack.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    double value;
    double hardmin;
    double softmin;
    double softmax;
    double hardmax;
} ZoneRangeCondition;
    
Zone* zoneCreate();
void zoneDelete(Zone* zone);
void zoneSave(PackStream* stream, Zone* zone);
void zoneLoad(PackStream* stream, Zone* zone);
void zoneCopy(Zone* source, Zone* destination);
void zoneIncludeCircleArea(Zone* zone, double value, double centerx, double centerz, double softradius, double hardradius);
void zoneExcludeCircleArea(Zone* zone, double centerx, double centerz, double softradius, double hardradius);

int zoneAddHeightRange(Zone* zone);
int zoneGetHeightRangeCount(Zone* zone);
int zoneGetHeightRange(Zone* zone, int position, ZoneRangeCondition* range);
int zoneSetHeightRange(Zone* zone, int position, ZoneRangeCondition* range);
int zoneAddHeightRangeQuick(Zone* zone, double value, double hardmin, double softmin, double softmax, double hardmax);

int zoneAddSlopeRange(Zone* zone);
int zoneGetSlopeRangeCount(Zone* zone);
int zoneGetSlopeRange(Zone* zone, int position, ZoneRangeCondition* range);
int zoneSetSlopeRange(Zone* zone, int position, ZoneRangeCondition* range);
int zoneAddSlopeRangeQuick(Zone* zone, double value, double hardmin, double softmin, double softmax, double hardmax);

double zoneGetValue(Zone* zone, Vector3 location, Vector3 normal);

#ifdef __cplusplus
}
#endif

#endif
