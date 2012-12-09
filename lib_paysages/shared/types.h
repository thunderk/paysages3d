#ifndef _PAYSAGES_TYPES_H_
#define _PAYSAGES_TYPES_H_

#include "../euclid.h"
#include "../color.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Renderer Renderer;

typedef struct LightDefinition LightDefinition;
typedef struct LightStatus LightStatus;

typedef Color (*f_RenderFragmentCallback)(struct Renderer* renderer, Vector3 location, void* data);

typedef struct
{
    int width;
    int height;
    int antialias;
    int quality;
} RenderParams;

typedef struct
{
    int length;
    int alloc_length;
    int item_size;
    int dirty;
    void* data;
} Array;

typedef struct
{
    Color base;
    double reflection;
    double shininess;
} SurfaceMaterial;

typedef void (*RenderCallbackStart)(int width, int height, Color background);
typedef void (*RenderCallbackDraw)(int x, int y, Color col);
typedef void (*RenderCallbackUpdate)(double progress);

typedef struct RenderArea RenderArea;

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

typedef struct
{
    Vector3 location;
    double yaw;
    double pitch;
    double roll;

    Vector3 target;
    Vector3 forward;
    Vector3 right;
    Vector3 up;

    double width;
    double height;
    double yfov;
    double xratio;
    double znear;
    double zfar;

    Matrix4 project;
    Matrix4 unproject;
} CameraDefinition;

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

#ifdef __cplusplus
}
#endif

#endif
