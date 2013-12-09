#ifndef _PAYSAGES_TYPES_H_
#define _PAYSAGES_TYPES_H_

#include "rendering_global.h"

#include "Color.h"
#include "Vector3.h"

typedef void* (*FuncObjectCreate)();
typedef void (*FuncObjectDelete)(void* object);
typedef void (*FuncObjectBind)(void* base, void* sub);
typedef struct {
    FuncObjectCreate create;
    FuncObjectDelete destroy;
    FuncObjectBind bind;
} StandardRenderer;

#endif
