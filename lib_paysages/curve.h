#ifndef _PAYSAGES_CURVE_H_
#define _PAYSAGES_CURVE_H_

#include "pack.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float position;
    float value;
} CurvePoint;
typedef struct Curve Curve;

Curve* curveCreate();
void curveDelete(Curve* curve);
void curveCopy(Curve* source, Curve* destination);

void curveSave(PackStream* stream, Curve* curve);
void curveLoad(PackStream* stream, Curve* curve);

void curveClear(Curve* curve);
int curveAddPoint(Curve* curve, CurvePoint* point);
int curveQuickAddPoint(Curve* curve, float position, float value);
int curveGetPointCount(Curve* curve);
void curveGetPoint(Curve* curve, int number, CurvePoint* point);
void curveSetPoint(Curve* curve, int number, CurvePoint* point);
void curveRemovePoint(Curve* curve, int number);
void curveValidate(Curve* curve);

float curveGetValue(Curve* curve, float position);

#ifdef __cplusplus
}
#endif

#endif
