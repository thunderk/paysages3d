#ifndef _PAYSAGES_ZONE_H_
#define _PAYSAGES_ZONE_H_

#include "shared/types.h"
#include "pack.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Zone Zone;

typedef struct
{
    float value;
    float hardmin;
    float softmin;
    float softmax;
    float hardmax;
} ZoneRangeCondition;
    
Zone* zoneCreate();
void zoneDelete(Zone* zone);
void zoneSave(PackStream* stream, Zone* zone);
void zoneLoad(PackStream* stream, Zone* zone);
void zoneCopy(Zone* source, Zone* destination);
void zoneIncludeCircleArea(Zone* zone, float value, float centerx, float centerz, float softradius, float hardradius);
void zoneExcludeCircleArea(Zone* zone, float centerx, float centerz, float softradius, float hardradius);

int zoneAddHeightRange(Zone* zone);
int zoneGetHeightRangeCount(Zone* zone);
int zoneGetHeightRange(Zone* zone, int position, ZoneRangeCondition* range);
int zoneSetHeightRange(Zone* zone, int position, ZoneRangeCondition* range);
int zoneAddHeightRangeQuick(Zone* zone, float value, float hardmin, float softmin, float softmax, float hardmax);

int zoneAddSlopeRange(Zone* zone);
int zoneGetSlopeRangeCount(Zone* zone);
int zoneGetSlopeRange(Zone* zone, int position, ZoneRangeCondition* range);
int zoneSetSlopeRange(Zone* zone, int position, ZoneRangeCondition* range);
int zoneAddSlopeRangeQuick(Zone* zone, float value, float hardmin, float softmin, float softmax, float hardmax);

float zoneGetValue(Zone* zone, Vector3 location, Vector3 normal);

#ifdef __cplusplus
}
#endif

#endif
