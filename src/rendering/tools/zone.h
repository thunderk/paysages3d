#ifndef _RENDERING_TOOLS_ZONE_H_
#define _RENDERING_TOOLS_ZONE_H_

/**
 * Definition of a geographic area.
 */

#include "../rendering_global.h"
#include "../tools/euclid.h"
#include "../tools/curve.h"

namespace paysages {
namespace system {
class PackStream;
}
}

typedef struct Zone Zone;

RENDERINGSHARED_EXPORT Zone* zoneCreate();
RENDERINGSHARED_EXPORT void zoneDelete(Zone* zone);
RENDERINGSHARED_EXPORT void zoneSave(PackStream* stream, Zone* zone);
RENDERINGSHARED_EXPORT void zoneLoad(PackStream* stream, Zone* zone);
RENDERINGSHARED_EXPORT void zoneCopy(Zone* source, Zone* destination);
RENDERINGSHARED_EXPORT void zoneClear(Zone* zone);

RENDERINGSHARED_EXPORT void zoneSetAbsoluteHeight(Zone* zone);
RENDERINGSHARED_EXPORT void zoneSetRelativeHeight(Zone* zone, double min, double middle, double max);

RENDERINGSHARED_EXPORT void zoneIncludeCircleArea(Zone* zone, double value, double centerx, double centerz, double softradius, double hardradius);
RENDERINGSHARED_EXPORT void zoneExcludeCircleArea(Zone* zone, double centerx, double centerz, double softradius, double hardradius);

RENDERINGSHARED_EXPORT void zoneGetHeightCurve(Zone* zone, Curve* curve);
RENDERINGSHARED_EXPORT void zoneSetHeightCurve(Zone* zone, Curve* curve);
RENDERINGSHARED_EXPORT void zoneAddHeightRangeQuick(Zone* zone, double value, double hardmin, double softmin, double softmax, double hardmax);

RENDERINGSHARED_EXPORT void zoneGetSlopeCurve(Zone* zone, Curve* curve);
RENDERINGSHARED_EXPORT void zoneSetSlopeCurve(Zone* zone, Curve* curve);
RENDERINGSHARED_EXPORT void zoneAddSlopeRangeQuick(Zone* zone, double value, double hardmin, double softmin, double softmax, double hardmax);

RENDERINGSHARED_EXPORT double zoneGetValue(Zone* zone, Vector3 location, Vector3 normal);

#endif
