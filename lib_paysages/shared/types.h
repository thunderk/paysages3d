#ifndef _PAYSAGES_TYPES_H_
#define _PAYSAGES_TYPES_H_

#include <stdio.h>
    
#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    double x;
    double y;
    double z;
} Vector3;

typedef struct
{
    double a;
    double b;
    double c;
    double d;
    double e;
    double f;
    double g;
    double h;
    double i;
    double j;
    double k;
    double l;
    double m;
    double n;
    double o;
    double p;
} Matrix4;

typedef struct
{
    double r;
    double g;
    double b;
    double a;
} Color;

struct RenderFragment;
struct Renderer;

typedef int(*f_RenderFragmentCallback)(struct RenderFragment*, struct Renderer* renderer, void* data);

typedef struct
{
    Vector3 location;
    Vector3 normal;
    Color color;
    f_RenderFragmentCallback callback;
    void* callback_data;
} Vertex;

typedef struct RenderFragment
{
    int x;
    int y;
    double z;
    Vertex vertex;
} RenderFragment;

typedef struct
{
    int width;
    int height;
} RenderContext;

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

typedef struct Zone Zone;

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
typedef RayCastingResult (*RayCastingFunction)(Vector3 start, Vector3 direction);

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

#ifdef __cplusplus
}
#endif

#endif
