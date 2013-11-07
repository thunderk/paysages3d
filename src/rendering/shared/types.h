#ifndef _PAYSAGES_TYPES_H_
#define _PAYSAGES_TYPES_H_

#include "../tools/euclid.h"
#include "../tools/color.h"

typedef struct Renderer Renderer;

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
typedef void (*FuncObjectCopy)(void* source, void* destination);
typedef void (*FuncObjectValidate)(void* object);
typedef void (*FuncObjectSave)(PackStream* stream, void* object);
typedef void (*FuncObjectLoad)(PackStream* stream, void* object);
typedef void (*FuncObjectBind)(void* base, void* sub);

typedef struct {
    FuncObjectCreate create;
    FuncObjectDelete destroy;
    FuncObjectCopy copy;
    FuncObjectValidate validate;
    FuncObjectSave save;
    FuncObjectLoad load;
} StandardDefinition;

typedef struct {
    FuncObjectCreate create;
    FuncObjectDelete destroy;
    FuncObjectBind bind;
} StandardRenderer;

#endif
