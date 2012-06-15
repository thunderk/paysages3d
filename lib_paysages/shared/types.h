#ifndef _PAYSAGES_TYPES_H_
#define _PAYSAGES_TYPES_H_

#include "../euclid.h"
#include "../color.h"
    
#ifdef __cplusplus
extern "C" {
#endif

struct RenderFragment;
struct Renderer;

typedef struct LightDefinition LightDefinition;
typedef struct LightStatus LightStatus;

typedef int(*f_RenderFragmentCallback)(struct RenderFragment*, struct Renderer* renderer, void* data);

typedef struct
{
    Vector3 location;
    Color color;
    f_RenderFragmentCallback callback;
    void* callback_data;
} Vertex;

typedef struct RenderFragment
{
    short int x;
    short int y;
    float z;
    Vertex vertex;
} RenderFragment;

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
    float reflection;
    float shininess;
} SurfaceMaterial;

typedef void (*RenderCallbackStart)(int width, int height, Color background);
typedef void (*RenderCallbackDraw)(int x, int y, Color col);
typedef void (*RenderCallbackUpdate)(float progress);

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
    float min_height;
    float max_height;
    float base_height;
} HeightInfo;

typedef struct
{
    Vector3 location;
    float yaw;
    float pitch;
    float roll;
    
    Vector3 target;
    Vector3 forward;
    Vector3 right;
    Vector3 up;
    
    float width;
    float height;
    float yfov;
    float xratio;
    float znear;
    float zfar;
    
    Matrix4 project;
    Matrix4 unproject;
} CameraDefinition;

#ifdef __cplusplus
}
#endif

#endif
