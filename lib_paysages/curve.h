#ifndef _PAYSAGES_CURVE_H_
#define _PAYSAGES_CURVE_H_

#include "pack.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    double position;
    double value;
} CurvePoint;
typedef struct Curve Curve;

Curve* curveCreate();
void curveDelete(Curve* curve);
void curveCopy(Curve* source, Curve* destination);

void curveSave(PackStream* stream, Curve* curve);
void curveLoad(PackStream* stream, Curve* curve);

void curveClear(Curve* curve);
void curveSetDefault(Curve* curve, double value);
int curveAddPoint(Curve* curve, CurvePoint* point);
int curveQuickAddPoint(Curve* curve, double position, double value);
int curveGetPointCount(Curve* curve);
void curveGetPoint(Curve* curve, int number, CurvePoint* point);
void curveSetPoint(Curve* curve, int number, CurvePoint* point);
void curveRemovePoint(Curve* curve, int number);
void curveValidate(Curve* curve);

double curveGetValue(Curve* curve, double position);

#ifdef __cplusplus
}
#endif

#endif
