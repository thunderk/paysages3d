#ifndef _PAYSAGES_TYPES_H_
#define _PAYSAGES_TYPES_H_

#include "rendering_global.h"

#include "Color.h"
#include "Vector3.h"

typedef struct
{
    int hit;
    Color hit_color;
    Vector3 hit_location;
} RayCastingResult;
typedef RayCastingResult (*FuncGeneralCastRay)(Renderer* renderer, Vector3 start, Vector3 direction);

typedef struct
{
    double min_height;
    double max_height;
    double base_height;
} HeightInfo;

typedef void* (*FuncObjectCreate)();
typedef void (*FuncObjectDelete)(void* object);
typedef void (*FuncObjectBind)(void* base, void* sub);
typedef struct {
    FuncObjectCreate create;
    FuncObjectDelete destroy;
    FuncObjectBind bind;
} StandardRenderer;

#endif
