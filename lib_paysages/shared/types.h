#ifndef _PAYSAGES_TYPES_H_
#define _PAYSAGES_TYPES_H_

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

#define MAX_COLORGRADATION_PARTS 10
typedef struct
{
    double start;
    Color col;
} _ColorGradationPart;
typedef struct
{
    int nbparts;
    _ColorGradationPart parts[MAX_COLORGRADATION_PARTS];
} ColorGradation;

struct RenderFragment;

typedef int(*f_RenderFragmentCallback)(struct RenderFragment*);

typedef struct
{
    Vector3 location;
    Vector3 normal;
    Color color;
    f_RenderFragmentCallback callback;
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

struct NoiseLevel
{
    double scaling;
    double height;
    double xoffset;
    double yoffset;
    double zoffset;
};
typedef struct NoiseLevel NoiseLevel;
typedef struct NoiseGenerator NoiseGenerator;

typedef struct Zone Zone;

typedef void (*PreviewCallbackResize)(int width, int height);
typedef void (*PreviewCallbackClear)(Color col);
typedef void (*PreviewCallbackDraw)(int x, int y, Color col);
typedef void (*PreviewCallbackUpdate)(double progress);
typedef int (*RenderProgressCallback)(double progress);

typedef struct
{
    int hit;
    Color hit_color;
    Vector3 hit_location;
} RayCastingResult;
typedef RayCastingResult (*RayCastingFunction)(Vector3 start, Vector3 direction);

#ifdef __cplusplus
}
#endif

#endif
