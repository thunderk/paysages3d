#include "shared/types.h"
#include "shared/constants.h"
#include "shared/functions.h"
#include "shared/system.h"

#include <stdlib.h>
#include <math.h>

#include "IL/il.h"
#include "IL/ilu.h"

int render_width;
int render_height;
int render_quality;
static int _pixel_count;
static Array* render_zone = NULL;
static RenderFragment* scanline_up;
static RenderFragment* scanline_down;
static int scanline_left;
static int scanline_right;
static Color background_color;
static volatile int _dirty_left;
static volatile int _dirty_right;
static volatile int _dirty_up;
static volatile int _dirty_down;
static volatile int _dirty_count;
static Mutex* _lock;

static volatile int _interrupt = 0;
static volatile int _progress_pixels;
static volatile double _progress = 0.0;
static volatile double _progress_step_start = 0.0;
static volatile double _progress_step_length = 1.0;

#define RENDER_INVERSE 1
#define RENDER_WIREFRAME 1

static void _previewResize(int width, int height) {}
static void _previewClear(Color col) {}
static void _previewDraw(int x, int y, Color col) {}
static void _previewUpdate(double progress) {}

static PreviewCallbackResize _cb_preview_resize = _previewResize;
static PreviewCallbackClear _cb_preview_clear = _previewClear;
static PreviewCallbackDraw _cb_preview_draw = _previewDraw;
static PreviewCallbackUpdate _cb_preview_update = _previewUpdate;

void renderSave(FILE* f)
{
}

void renderLoad(FILE* f)
{
}

void renderSetSize(int width, int height)
{
    int x;
    int y;

    if (render_zone != NULL)
    {
        /* Delete previous render zone */
        for (x = 0; x < render_width; x++)
        {
            for (y = 0; y < render_height; y++)
            {
                arrayDelete(render_zone + (y * render_width + x));
            }
        }
        free(render_zone);
        free(scanline_up);
        free(scanline_down);
    }

    if (_lock == NULL)
    {
        _lock = mutexCreate();
    }

    render_width = width;
    render_height = height;
    render_zone = malloc(sizeof(Array) * width * height);

    scanline_left = 0;
    scanline_right = render_width - 1;
    scanline_up = malloc(sizeof(RenderFragment) * width);
    scanline_down = malloc(sizeof(RenderFragment) * width);

    _dirty_left = render_width;
    _dirty_right = -1;
    _dirty_down = render_height;
    _dirty_up = -1;
    _dirty_count = 0;
    
    _pixel_count = render_width * render_height;

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            arrayCreate(render_zone + (y * width + x), sizeof(RenderFragment));
        }
    }

    _cb_preview_resize(render_width, render_height);
    _cb_preview_clear(background_color);
}

void renderSetQuality(int quality)
{
    if (quality < 1)
    {
        render_quality = 1;
    }
    else
    {
        render_quality = quality;
    }
}

void renderClear()
{
    int x;
    int y;

    for (x = 0; x < render_width; x++)
    {
        for (y = 0; y < render_height; y++)
        {
            arrayClear(render_zone + (y * render_width + x));
        }
    }

    scanline_left = 0;
    scanline_right = render_width - 1;
    
    _progress = 0.0;
    _interrupt = 0;

    _cb_preview_clear(background_color);
    _cb_preview_update(_progress * _progress_step_length + _progress_step_start);

    _dirty_left = render_width;
    _dirty_right = -1;
    _dirty_down = render_height;
    _dirty_up = -1;
    _dirty_count = 0;
}

void renderInterrupt()
{
    _interrupt = 1;
}

void renderSetBackgroundColor(Color* col)
{
    background_color = *col;
}

double renderGetPrecision(Vector3 location)
{
    Vector3 projected;

    projected = cameraProject(location);
    projected.x += 1.0;
    //projected.y += 1.0;

    return v3Norm(v3Sub(cameraUnproject(projected), location)); // / (double)render_quality;
}

int _sortRenderFragment(void const* a, void const* b)
{
    double za, zb;
    za = ((RenderFragment*)a)->z;
    zb = ((RenderFragment*)b)->z;
    if (za > zb)
    {
        return 1;
    }
    else if (za < zb)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

static Color _getPixelColor(Array* pixel_data)
{
    RenderFragment* fragment;
    Color result = background_color;
    int i;

    if (pixel_data->length > 0)
    {
        for (i = 0; i < pixel_data->length; i++)
        {
            fragment = ((RenderFragment*)pixel_data->data) + i;
            colorMask(&result, &(fragment->vertex.color));
        }
    }

    return result;
}

static inline void _setDirtyPixel(Array* pixel_data, int x, int y)
{
    pixel_data->dirty = 1;
    if (x < _dirty_left)
    {
        _dirty_left = x;
    }
    if (x > _dirty_right)
    {
        _dirty_right = x;
    }
    if (y < _dirty_down)
    {
        _dirty_down = y;
    }
    if (y > _dirty_up)
    {
        _dirty_up = y;
    }

    _dirty_count++;
}

static void _processDirtyPixels()
{
    Color col;
    Array* pixel_data;
    int x, y;

    for (y = _dirty_down; y <= _dirty_up; y++)
    {
        for (x = _dirty_left; x <= _dirty_right; x++)
        {
            pixel_data = render_zone + y * render_width + x;
            if (pixel_data->dirty)
            {
                col = _getPixelColor(pixel_data);
                _cb_preview_draw(x, y, col);
                pixel_data->dirty = 0;
            }
        }
    }

    _cb_preview_update(_progress * _progress_step_length + _progress_step_start);

    _dirty_left = render_width;
    _dirty_right = -1;
    _dirty_down = render_height;
    _dirty_up = -1;
    _dirty_count = 0;
}

void renderAddFragment(RenderFragment* fragment)
{
    Array* pixel_data;
    int x = fragment->x;
    int y = fragment->y;
    double z = fragment->z;

    int i, dirty;
    int fragments_count;
    RenderFragment* fragments;

    dirty = 0;
    if (x >= 0 && x < render_width && y >= 0 && y < render_height && z > 1.0)
    {
        pixel_data = render_zone + (y * render_width + x);
        fragments = (RenderFragment*)pixel_data->data;
        fragments_count = pixel_data->length;

        if (fragments_count == 0)
        {
            arrayAppend(pixel_data, fragment);
            dirty = 1;
        }
        else if (fragments[0].z > z)
        {
            if (fragments[0].vertex.color.a < 1.0)
            {
                arrayInsert(pixel_data, fragment, 0);
                dirty = 1;
            }
        }
        else
        {
            for (i = 1; i <= fragments_count; i++)
            {
                if ((i == fragments_count) || (fragments[i].z > z))
                {
                    if (fragment->vertex.color.a > 0.999999)
                    {
                        if (i > 1)
                        {
                            arrayLStrip(pixel_data, i - 1);
                        }
                        arrayReplace(pixel_data, fragment, 0);
                    }
                    else if (i == fragments_count)
                    {
                        arrayAppend(pixel_data, fragment);
                    }
                    else
                    {
                        arrayInsert(pixel_data, fragment, i);
                    }
                    dirty = 1;
                    break;
                }
            }
        }

        if (dirty)
        {
            _setDirtyPixel(pixel_data, x, y);
        }
    }
}

void renderPushFragment(int x, int y, double z, Vertex* vertex)
{
    RenderFragment fragment;

    fragment.x = x;
    fragment.y = y;
    fragment.z = z;
    fragment.vertex = *vertex;

    renderAddFragment(&fragment);
}

static void __vertexGetDiff(Vertex* v1, Vertex* v2, Vertex* result)
{
    result->location.x = v2->location.x - v1->location.x;
    result->location.y = v2->location.y - v1->location.y;
    result->location.z = v2->location.z - v1->location.z;
    result->normal.x = v2->normal.x - v1->normal.x;
    result->normal.y = v2->normal.y - v1->normal.y;
    result->normal.z = v2->normal.z - v1->normal.z;
    result->color.r = v2->color.r - v1->color.r;
    result->color.g = v2->color.g - v1->color.g;
    result->color.b = v2->color.b - v1->color.b;
    result->color.a = v2->color.a - v1->color.a;
    result->callback = v1->callback;
}

static void __vertexInterpolate(Vertex* v1, Vertex* diff, double value, Vertex* result)
{
    result->location.x = v1->location.x + diff->location.x * value;
    result->location.y = v1->location.y + diff->location.y * value;
    result->location.z = v1->location.z + diff->location.z * value;
    result->normal.x = v1->normal.x + diff->normal.x * value;
    result->normal.y = v1->normal.y + diff->normal.y * value;
    result->normal.z = v1->normal.z + diff->normal.z * value;
    result->color.r = v1->color.r + diff->color.r * value;
    result->color.g = v1->color.g + diff->color.g * value;
    result->color.b = v1->color.b + diff->color.b * value;
    result->color.a = v1->color.a + diff->color.a * value;
    result->callback = v1->callback;
}

static void __pushScanLinePoint(RenderFragment point)
{
    if (point.x < 0 || point.x >= render_width)
    {
        return;
    }

    if (point.x > scanline_right)
    {
        scanline_right = point.x;
        scanline_up[scanline_right] = point;
        scanline_down[scanline_right] = point;
        if (point.x < scanline_left)
        {
            scanline_left = point.x;
        }
    }
    else if (point.x < scanline_left)
    {
        scanline_left = point.x;
        scanline_up[scanline_left] = point;
        scanline_down[scanline_left] = point;
    }
    else
    {
        if (point.y > scanline_up[point.x].y)
        {
            scanline_up[point.x] = point;
        }
        if (point.y < scanline_down[point.x].y)
        {
            scanline_down[point.x] = point;
        }
    }
}

static void __pushScanLineEdge(Vector3 v1, Vector3 v2, Vertex* vertex1, Vertex* vertex2)
{
    double dx, dy, dz, fx;
    Vertex diff;
    int startx = lround(v1.x);
    int endx = lround(v2.x);
    int curx;
    RenderFragment fragment;

    if (endx < startx)
    {
        __pushScanLineEdge(v2, v1, vertex2, vertex1);
    }
    else if (endx < 0 || startx >= render_width)
    {
        return;
    }
    else if (startx == endx)
    {
        fragment.x = startx;
        fragment.y = lround(v1.y);
        fragment.z = v1.z;
        fragment.vertex = *vertex1;

        __pushScanLinePoint(fragment);

        fragment.x = endx;
        fragment.y = lround(v2.y);
        fragment.z = v2.z;
        fragment.vertex = *vertex2;

        __pushScanLinePoint(fragment);
    }
    else
    {
        if (startx < 0)
        {
            startx = 0;
        }
        if (endx >= render_width)
        {
            endx = render_width - 1;
        }

        dx = v2.x - v1.x;
        dy = v2.y - v1.y;
        dz = v2.z - v1.z;
        __vertexGetDiff(vertex1, vertex2, &diff);
        for (curx = startx; curx <= endx; curx++)
        {
            fx = (double)curx + 0.5;
            if (fx < v1.x)
            {
                fx = v1.x;
            }
            else if (fx > v2.x)
            {
                fx = v2.x;
            }
            fx = fx - v1.x;
            fragment.x = curx;
            fragment.y = lround(v1.y + dy * fx / dx);
            fragment.z = v1.z + dz * fx / dx;
            __vertexInterpolate(vertex1, &diff, fx / dx, &(fragment.vertex));

            __pushScanLinePoint(fragment);
        }
    }
}

static void __clearScanLines()
{
    int x;
    for (x = scanline_left; x <= scanline_right; x++)
    {
        scanline_up[x].y = -1;
        scanline_down[x].y = render_height;
    }
    scanline_left = render_width;
    scanline_right = -1;
}

static void __renderScanLines()
{
    int x, starty, endy, cury;
    Vertex diff;
    double dy, dz, fy;
    RenderFragment up, down, current;

    if (scanline_right > 0)
    {
        for (x = scanline_left; x <= scanline_right; x++)
        {
            up = scanline_up[x];
            down = scanline_down[x];

            starty = down.y;
            endy = up.y;

            if (endy < 0 || starty >= render_height)
            {
                continue;
            }

            if (starty < 0)
            {
                starty = 0;
            }
            if (endy >= render_height)
            {
                endy = render_height - 1;
            }

            dy = (double)(up.y - down.y);
            dz = up.z - down.z;
            __vertexGetDiff(&down.vertex, &up.vertex, &diff);

            current.x = x;
            for (cury = starty; cury <= endy; cury++)
            {
                fy = (double)cury - down.y;

                current.y = cury;
                current.z = down.z + dz * fy / dy;
                __vertexInterpolate(&down.vertex, &diff, fy / dy, &current.vertex);

#ifdef RENDER_WIREFRAME
                if (cury == starty || cury == endy)
                {
                    current.vertex.color = COLOR_RED;
                }
#endif

                renderAddFragment(&current);
            }
        }
    }
}

void renderPushTriangle(Vertex* v1, Vertex* v2, Vertex* v3)
{
    Vector3 p1, p2, p3;
    double limit_width = (double)(render_width - 1);
    double limit_height = (double)(render_height - 1);

    p1 = cameraProject(v1->location);
    p2 = cameraProject(v2->location);
    p3 = cameraProject(v3->location);

    /* Filter if outside screen */
    if (p1.z < 1.0 || p2.z < 1.0 || p3.z < 1.0 || (p1.x < 0.0 && p2.x < 0.0 && p3.x < 0.0) || (p1.y < 0.0 && p2.y < 0.0 && p3.y < 0.0) || (p1.x > limit_width && p2.x > limit_width && p3.x > limit_width) || (p1.y > limit_height && p2.y > limit_height && p3.y > limit_height))
    {
        return;
    }

    __clearScanLines();
    __pushScanLineEdge(p1, p2, v1, v2);
    __pushScanLineEdge(p2, p3, v2, v3);
    __pushScanLineEdge(p3, p1, v3, v1);
    mutexAcquire(_lock);
    __renderScanLines();
    mutexRelease(_lock);
}

void renderPushQuad(Vertex* v1, Vertex* v2, Vertex* v3, Vertex* v4)
{
    renderPushTriangle(v2, v3, v1);
    renderPushTriangle(v4, v1, v3);
}

typedef struct {
    int startx;
    int endx;
    int starty;
    int endy;
    int finished;
    int interrupt;
    Thread* thread;
} RenderChunk;

void* _renderPostProcessChunk(void* data)
{
    int x, y, i;
    int dirty;
    Array* pixel_data;
    RenderFragment* fragments;
    RenderChunk* chunk = (RenderChunk*)data;

#ifdef RENDER_INVERSE
    for (y = render_height - 1 - chunk->starty; y >= render_height - 1 - chunk->endy; y--)
#else
    for (y = chunk->starty; y <= chunk->endy; y++)
#endif
    {
        for (x = chunk->startx; x <= chunk->endx; x++)
        {
            pixel_data = render_zone + (y * render_width + x);
            fragments = (RenderFragment*)pixel_data->data;
            dirty = 0;
            for (i = 0; i < pixel_data->length; i++)
            {
                if (fragments[i].vertex.callback)
                {
                    if (fragments[i].vertex.callback(fragments + i))
                    {
                        /* TODO Store over-exposure */
                        colorNormalize(&fragments[i].vertex.color);
                        dirty = 1;
                    }
                }
            }
            if (dirty)
            {
                mutexAcquire(_lock);
                _setDirtyPixel(pixel_data, x, y);
                mutexRelease(_lock);
            }
            _progress_pixels++;
        }
        if (chunk->interrupt)
        {
            break;
        }
    }

    chunk->finished = 1;
    return NULL;
}

#define MAX_CHUNKS 8
void renderPostProcess(int nbchunks)
{
    volatile RenderChunk chunks[MAX_CHUNKS];
    int i;
    int x, y, dx, dy, nx, ny;
    int loops, running;

    if (nbchunks > MAX_CHUNKS)
    {
        nbchunks = MAX_CHUNKS;
    }
    if (nbchunks < 1)
    {
        nbchunks = 1;
    }

    nx = 10;
    ny = 10;
    dx = render_width / nx;
    dy = render_height / ny;
    x = 0;
    y = 0;
    _progress_pixels = 0;

    for (i = 0; i < nbchunks; i++)
    {
        chunks[i].thread = NULL;
    }

    running = 0;
    loops = 0;
    while ((y < ny && !_interrupt) || running > 0)
    {
        timeSleepMs(100);

        for (i = 0; i < nbchunks; i++)
        {
            if (chunks[i].thread)
            {
                if (chunks[i].finished)
                {
                    threadJoin(chunks[i].thread);
                    chunks[i].thread = NULL;
                    running--;
                }
                else if (_interrupt)
                {
                    chunks[i].interrupt = 1;
                }
            }
            
            if (y < ny && !chunks[i].thread && !_interrupt)
            {
                chunks[i].finished = 0;
                chunks[i].interrupt = 0;
                chunks[i].startx = x * dx;
                if (x == nx)
                {
                    chunks[i].endx = render_width - 1;
                }
                else
                {
                    chunks[i].endx = (x + 1) * dx - 1;
                }
                chunks[i].starty = y * dy;
                if (y == ny)
                {
                    chunks[i].endy = render_height - 1;
                }
                else
                {
                    chunks[i].endy = (y + 1) * dy - 1;
                }

                chunks[i].thread = threadCreate(_renderPostProcessChunk, (void*)(chunks + i));
                running++;

                if (++x >= nx)
                {
                    y++;
                    x = 0;
                }
            }
        }

        if (++loops >= 10)
        {
            mutexAcquire(_lock);
            _progress = (double)_progress_pixels / (double)_pixel_count;
            _processDirtyPixels();
            mutexRelease(_lock);

            loops = 0;
        }
    }

    _progress = 1.0;
    _processDirtyPixels();
}

void renderUpdate()
{
    mutexAcquire(_lock);
    _processDirtyPixels();
    mutexRelease(_lock);
}

void renderSaveToFile(const char* path)
{
    ILuint image_id;
    ilGenImages(1, &image_id);
    ilBindImage(image_id);
    Color result;
    ILuint x, y;
    ILuint rgba;
    ILuint data[render_height * render_width];
    ILenum error;
    Array* pixel_data;

    for (y = 0; y < render_height; y++)
    {
        for (x = 0; x < render_width; x++)
        {
            pixel_data = render_zone + (y * render_width + x);
            result = _getPixelColor(pixel_data);
            rgba = colorTo32BitRGBA(&result);
            data[y * render_width + x] = rgba;
        }
    }

    ilTexImage((ILuint)render_width, (ILuint)render_height, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, data);
    ilSaveImage(path);

    ilDeleteImages(1, &image_id);

    while ((error=ilGetError()) != IL_NO_ERROR)
    {
        fprintf(stderr, "IL ERROR : %s\n", iluErrorString(error));
    }
}

void renderSetPreviewCallbacks(PreviewCallbackResize resize, PreviewCallbackClear clear, PreviewCallbackDraw draw, PreviewCallbackUpdate update)
{
    _cb_preview_resize = resize ? resize : _previewResize;
    _cb_preview_clear = clear ? clear : _previewClear;
    _cb_preview_draw = draw ? draw : _previewDraw;
    _cb_preview_update = update ? update : _previewUpdate;
    _cb_preview_resize(render_width, render_height);
    /* TODO Send all pixels ? */
}

int renderSetNextProgressStep(double start, double end)
{
    if (_interrupt)
    {
        return 0;
    }
    else
    {
        _progress = 0.0;
        _progress_step_start = start;
        _progress_step_length = end - start;
        return 1;
    }
}

int renderTellProgress(double progress)
{
    if (_interrupt)
    {
        return 0;
    }
    else
    {
        _progress = progress;
        return 1;
    }
}
