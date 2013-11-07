#ifndef _PAYSAGES_TOOLS_CURVE_H_
#define _PAYSAGES_TOOLS_CURVE_H_

#include "../rendering_global.h"

namespace paysages {
namespace system {class PackStream;}
}

typedef struct {
    double position;
    double value;
} CurvePoint;
typedef struct Curve Curve;

RENDERINGSHARED_EXPORT Curve* curveCreate();
RENDERINGSHARED_EXPORT void curveDelete(Curve* curve);
RENDERINGSHARED_EXPORT void curveCopy(Curve* source, Curve* destination);

RENDERINGSHARED_EXPORT void curveSave(PackStream* stream, Curve* curve);
RENDERINGSHARED_EXPORT void curveLoad(PackStream* stream, Curve* curve);

RENDERINGSHARED_EXPORT void curveClear(Curve* curve);
RENDERINGSHARED_EXPORT void curveSetDefault(Curve* curve, double value);
RENDERINGSHARED_EXPORT int curveAddPoint(Curve* curve, CurvePoint* point);
RENDERINGSHARED_EXPORT int curveQuickAddPoint(Curve* curve, double position, double value);
RENDERINGSHARED_EXPORT int curveGetPointCount(Curve* curve);
RENDERINGSHARED_EXPORT void curveGetPoint(Curve* curve, int number, CurvePoint* point);
RENDERINGSHARED_EXPORT void curveSetPoint(Curve* curve, int number, CurvePoint* point);
RENDERINGSHARED_EXPORT void curveRemovePoint(Curve* curve, int number);
RENDERINGSHARED_EXPORT void curveValidate(Curve* curve);

RENDERINGSHARED_EXPORT double curveGetValue(Curve* curve, double position);

#endif
