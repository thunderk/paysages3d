#include "RenderArea.h"

#include "Vector3.h"
#include "ColorProfile.h"
#include "Mutex.h"
#include "CameraDefinition.h"
#include "SoftwareRenderer.h"
#include "Thread.h"
#include "PictureWriter.h"

struct RenderFragment
{
    struct
    {
        unsigned char dirty    : 1;
        unsigned char edge     : 1;
        unsigned char callback : 6;
    } flags;
    union
    {
        struct {
            double x;
            double y;
            double z;
        } location;
        struct {
            double r;
            double g;
            double b;
        } color;
    } data;
    double z;
};

typedef struct
{
    int x;
    int y;
    Vector3 pixel;
    Vector3 location;
    int callback;
} ScanPoint;

struct FragmentCallback
{
    RenderArea::f_RenderFragmentCallback function;
    void* data;
};

typedef struct
{
    ScanPoint* up;
    ScanPoint* down;
    int left;
    int right;
} RenderScanlines;

typedef struct
{
    int startx;
    int endx;
    int starty;
    int endy;
    int finished;
    int interrupt;
    int pixel_done;
    Thread* thread;
    RenderArea* area;
} RenderChunk;

static void _callbackStart(int, int, Color) {}
static void _callbackDraw(int, int, Color) {}
static void _callbackUpdate(double) {}

RenderArea::RenderArea(SoftwareRenderer* renderer)
{
    this->renderer = renderer;
    this->hdr_mapping = new ColorProfile;
    this->params.width = 1;
    this->params.height = 1;
    this->params.antialias = 1;
    this->params.quality = 5;
    this->pixel_count = 1;
    this->pixels = new RenderFragment[1];
    this->fragment_callbacks_count = 0;
    this->fragment_callbacks = new FragmentCallback[64];
    this->background_color = COLOR_TRANSPARENT;
    this->dirty_left = 1;
    this->dirty_right = -1;
    this->dirty_down = 1;
    this->dirty_up = -1;
    this->dirty_count = 0;
    this->lock = new Mutex();
    this->callback_start = _callbackStart;
    this->callback_draw = _callbackDraw;
    this->callback_update = _callbackUpdate;
}

RenderArea::~RenderArea()
{
    delete hdr_mapping;
    delete lock;
    delete[] fragment_callbacks;
    delete[] pixels;
}

void RenderArea::setAllDirty()
{
    dirty_left = 0;
    dirty_right = params.width * params.antialias - 1;
    dirty_down = 0;
    dirty_up = params.height * params.antialias - 1;
}

void RenderArea::setParams(RenderParams params)
{
    int width, height;

    width = params.width * params.antialias;
    height = params.height * params.antialias;

    params = params;
    delete[] pixels;
    pixels = new RenderFragment[width * height];
    pixel_count = width * height;

    dirty_left = width;
    dirty_right = -1;
    dirty_down = height;
    dirty_up = -1;
    dirty_count = 0;

    clear();
}

void RenderArea::setToneMapping(const ColorProfile &profile)
{
    profile.copy(hdr_mapping);
    setAllDirty();
    update();
}

void RenderArea::setBackgroundColor(const Color &col)
{
    background_color = col;
}

void RenderArea::clear()
{
    RenderFragment* pixel;
    int x;
    int y;

    fragment_callbacks_count = 1;
    fragment_callbacks[0].function = NULL;
    fragment_callbacks[0].data = NULL;

    for (x = 0; x < params.width * params.antialias; x++)
    {
        for (y = 0; y < params.height * params.antialias; y++)
        {
            pixel = pixels + (y * params.width * params.antialias + x);
            pixel->z = -100000000.0;
            pixel->flags.dirty = 0;
            pixel->flags.callback = 0;
            pixel->data.color.r = background_color.r;
            pixel->data.color.g = background_color.g;
            pixel->data.color.b = background_color.b;
        }
    }

    callback_start(params.width, params.height, background_color);

    dirty_left = params.width * params.antialias;
    dirty_right = -1;
    dirty_down = params.height * params.antialias;
    dirty_up = -1;
    dirty_count = 0;
}

static inline void _setDirtyPixel(RenderArea* area, int x, int y)
{
    if (x < area->dirty_left)
    {
        area->dirty_left = x;
    }
    if (x > area->dirty_right)
    {
        area->dirty_right = x;
    }
    if (y < area->dirty_down)
    {
        area->dirty_down = y;
    }
    if (y > area->dirty_up)
    {
        area->dirty_up = y;
    }

    area->dirty_count++;
}

static inline Color _getFinalPixel(RenderArea* area, int x, int y)
{
    Color result, col;
    int sx, sy;
    RenderFragment* pixel_data;

    result.r = result.g = result.b = 0.0;
    result.a = 1.0;
    for (sx = 0; sx < area->params.antialias; sx++)
    {
        for (sy = 0; sy < area->params.antialias; sy++)
        {
            pixel_data = area->pixels + (y * area->params.antialias + sy) * area->params.width * area->params.antialias + (x * area->params.antialias + sx);
            if (pixel_data->flags.dirty)
            {
                if (pixel_data->flags.edge)
                {
                    col = COLOR_GREY;
                }
                else
                {
                    col = COLOR_WHITE;
                }
            }
            else
            {
                col.r = pixel_data->data.color.r;
                col.g = pixel_data->data.color.g;
                col.b = pixel_data->data.color.b;
            }
            result.r += col.r / (double)(area->params.antialias * area->params.antialias);
            result.g += col.g / (double)(area->params.antialias * area->params.antialias);
            result.b += col.b / (double)(area->params.antialias * area->params.antialias);

        }
    }

    return area->hdr_mapping->apply(result);
}

void RenderArea::processDirtyPixels()
{
    int x, y;
    int down, up, left, right;

    down = dirty_down / params.antialias;
    up = dirty_up / params.antialias;
    left = dirty_left / params.antialias;
    right = dirty_right / params.antialias;

    for (y = down; y <= up; y++)
    {
        for (x = left; x <= right; x++)
        {
            callback_draw(x, y, _getFinalPixel(this, x, y));
        }
    }

    callback_update(renderer->render_progress);

    dirty_left = params.width * params.antialias;
    dirty_right = -1;
    dirty_down = params.height * params.antialias;
    dirty_up = -1;
    dirty_count = 0;
}

void RenderArea::update()
{
    lock->acquire();
    processDirtyPixels();
    lock->release();
}

static inline unsigned int _pushCallback(RenderArea* area, FragmentCallback callback)
{
    int i;
    for (i = 0; i < area->fragment_callbacks_count; i++)
    {
        if (area->fragment_callbacks[i].function == callback.function && area->fragment_callbacks[i].data == callback.data)
        {
            return i;
        }
    }

    if (area->fragment_callbacks_count >= 64)
    {
        return 0;
    }
    else
    {
        area->fragment_callbacks[area->fragment_callbacks_count].function = callback.function;
        area->fragment_callbacks[area->fragment_callbacks_count].data = callback.data;
        return area->fragment_callbacks_count++;
    }
}

void RenderArea::pushFragment(int x, int y, double z, int edge, Vector3 location, int callback)
{
    RenderFragment* pixel_data;

    if (x >= 0 && x < params.width * params.antialias && y >= 0 && y < params.height * params.antialias && z > 1.0)
    {
        pixel_data = pixels + (y * params.width * params.antialias + x);

        if (z > pixel_data->z)
        {
            pixel_data->flags.dirty = (unsigned char)1;
            pixel_data->flags.edge = (unsigned char)edge;
            pixel_data->flags.callback = (unsigned char)callback;
            pixel_data->data.location.x = location.x;
            pixel_data->data.location.y = location.y;
            pixel_data->data.location.z = location.z;
            pixel_data->z = z;
            _setDirtyPixel(this, x, y);
        }
    }
}

static void _scanGetDiff(ScanPoint* v1, ScanPoint* v2, ScanPoint* result)
{
    result->pixel.x = v2->pixel.x - v1->pixel.x;
    result->pixel.y = v2->pixel.y - v1->pixel.y;
    result->pixel.z = v2->pixel.z - v1->pixel.z;
    result->location.x = v2->location.x - v1->location.x;
    result->location.y = v2->location.y - v1->location.y;
    result->location.z = v2->location.z - v1->location.z;
    result->callback = v1->callback;
}

static void _scanInterpolate(CameraDefinition* camera, ScanPoint* v1, ScanPoint* diff, double value, ScanPoint* result)
{
    double v1depth = camera->getRealDepth(v1->pixel);
    double v2depth = camera->getRealDepth(v1->pixel.add(diff->pixel));
    double factor = ((1.0 - value) / v1depth + value / v2depth);

    result->pixel.x = v1->pixel.x + diff->pixel.x * value;
    result->pixel.y = v1->pixel.y + diff->pixel.y * value;
    result->pixel.z = v1->pixel.z + diff->pixel.z * value;
    result->location.x = ((1.0 - value) * (v1->location.x / v1depth) + value * (v1->location.x + diff->location.x) / v2depth) / factor;
    result->location.y = ((1.0 - value) * (v1->location.y / v1depth) + value * (v1->location.y + diff->location.y) / v2depth) / factor;
    result->location.z = ((1.0 - value) * (v1->location.z / v1depth) + value * (v1->location.z + diff->location.z) / v2depth) / factor;
    result->callback = v1->callback;
}

static void _pushScanPoint(RenderArea* area, RenderScanlines* scanlines, ScanPoint* point)
{
    point->x = (int)floor(point->pixel.x);
    point->y = (int)floor(point->pixel.y);

    if (point->x < 0 || point->x >= area->params.width * area->params.antialias)
    {
        return;
    }

    if (point->x > scanlines->right)
    {
        scanlines->right = point->x;
        scanlines->up[scanlines->right] = *point;
        scanlines->down[scanlines->right] = *point;
        if (point->x < scanlines->left)
        {
            scanlines->left = point->x;
        }
    }
    else if (point->x < scanlines->left)
    {
        scanlines->left = point->x;
        scanlines->up[scanlines->left] = *point;
        scanlines->down[scanlines->left] = *point;
    }
    else
    {
        if (point->y > scanlines->up[point->x].y)
        {
            scanlines->up[point->x] = *point;
        }
        if (point->y < scanlines->down[point->x].y)
        {
            scanlines->down[point->x] = *point;
        }
    }
}

static void _pushScanLineEdge(RenderArea* area, RenderScanlines* scanlines, ScanPoint* point1, ScanPoint* point2)
{
    double dx, fx;
    ScanPoint diff, point;
    int startx = lround(point1->pixel.x);
    int endx = lround(point2->pixel.x);
    int curx;

    if (endx < startx)
    {
        _pushScanLineEdge(area, scanlines, point2, point1);
    }
    else if (endx < 0 || startx >= area->params.width * area->params.antialias)
    {
        return;
    }
    else if (startx == endx)
    {
        _pushScanPoint(area, scanlines, point1);
        _pushScanPoint(area, scanlines, point2);
    }
    else
    {
        if (startx < 0)
        {
            startx = 0;
        }
        if (endx >= area->params.width * area->params.antialias)
        {
            endx = area->params.width * area->params.antialias - 1;
        }

        dx = point2->pixel.x - point1->pixel.x;
        _scanGetDiff(point1, point2, &diff);
        for (curx = startx; curx <= endx; curx++)
        {
            fx = (double)curx + 0.5;
            if (fx < point1->pixel.x)
            {
                fx = point1->pixel.x;
            }
            else if (fx > point2->pixel.x)
            {
                fx = point2->pixel.x;
            }
            fx = fx - point1->pixel.x;
            _scanInterpolate(area->renderer->render_camera, point1, &diff, fx / dx, &point);

            /*point.pixel.x = (double)curx;*/

            _pushScanPoint(area, scanlines, &point);
        }
    }
}

static void _renderScanLines(RenderArea* area, RenderScanlines* scanlines)
{
    int x, starty, endy, cury;
    ScanPoint diff;
    double dy, fy;
    ScanPoint up, down, current;

    if (scanlines->right > 0)
    {
        for (x = scanlines->left; x <= scanlines->right; x++)
        {
            up = scanlines->up[x];
            down = scanlines->down[x];

            starty = down.y;
            endy = up.y;

            if (endy < 0 || starty >= area->params.height * area->params.antialias)
            {
                continue;
            }

            if (starty < 0)
            {
                starty = 0;
            }
            if (endy >= area->params.height * area->params.antialias)
            {
                endy = area->params.height * area->params.antialias - 1;
            }

            dy = up.pixel.y - down.pixel.y;
            _scanGetDiff(&down, &up, &diff);

            current.x = x;
            for (cury = starty; cury <= endy; cury++)
            {
                fy = (double)cury + 0.5;
                if (fy < down.pixel.y)
                {
                    fy = down.pixel.y;
                }
                else if (fy > up.pixel.y)
                {
                    fy = up.pixel.y;
                }
                fy = fy - down.pixel.y;

                current.y = cury;
                _scanInterpolate(area->renderer->render_camera, &down, &diff, fy / dy, &current);

                area->pushFragment(current.x, current.y, current.pixel.z, (cury == starty || cury == endy), current.location, current.callback);
            }
        }
    }
}

void RenderArea::pushTriangle(Vector3 pixel1, Vector3 pixel2, Vector3 pixel3, Vector3 location1, Vector3 location2, Vector3 location3, f_RenderFragmentCallback callback, void* callback_data)
{
    FragmentCallback fragment_callback = {callback, callback_data};
    ScanPoint point1, point2, point3;
    double limit_width = (double)(params.width * params.antialias - 1);
    double limit_height = (double)(params.height * params.antialias - 1);

    /* Filter if outside screen */
    if (pixel1.z < 1.0 || pixel2.z < 1.0 || pixel3.z < 1.0 || (pixel1.x < 0.0 && pixel2.x < 0.0 && pixel3.x < 0.0) || (pixel1.y < 0.0 && pixel2.y < 0.0 && pixel3.y < 0.0) || (pixel1.x > limit_width && pixel2.x > limit_width && pixel3.x > limit_width) || (pixel1.y > limit_height && pixel2.y > limit_height && pixel3.y > limit_height))
    {
        return;
    }

    /* Prepare fragment callback */
    lock->acquire();
    point1.callback = _pushCallback(this, fragment_callback);
    lock->release();

    /* Prepare vertices */
    point1.pixel = pixel1;
    point1.location = location1;

    point2.pixel = pixel2;
    point2.location = location2;
    point2.callback = point1.callback;

    point3.pixel = pixel3;
    point3.location = location3;
    point3.callback = point1.callback;

    /* Prepare scanlines */
    RenderScanlines scanlines;
    int x;
    int width = params.width * params.antialias;
    scanlines.left = width;
    scanlines.right = -1;
    scanlines.up = new ScanPoint[width];
    scanlines.down = new ScanPoint[width];
    for (x = 0; x < width; x++)
    {
        /* TODO Do not initialize whole width each time, init only when needed on point push */
        scanlines.up[x].y = -1;
        scanlines.down[x].y = params.height * params.antialias;
    }

    /* Render edges in scanlines */
    _pushScanLineEdge(this, &scanlines, &point1, &point2);
    _pushScanLineEdge(this, &scanlines, &point2, &point3);
    _pushScanLineEdge(this, &scanlines, &point3, &point1);

    /* Commit scanlines to area */
    lock->acquire();
    _renderScanLines(this, &scanlines);
    lock->release();

    /* Free scalines */
    free(scanlines.up);
    free(scanlines.down);
}

Color RenderArea::getPixel(int x, int y)
{
    Color result;

    lock->acquire();
    result = _getFinalPixel(this, x, y);
    lock->release();

    return result;
}

void* _renderPostProcessChunk(void* data)
{
    int x, y;
    RenderFragment* fragment;
    RenderChunk* chunk = (RenderChunk*)data;

    for (x = chunk->startx; x <= chunk->endx; x++)
{
        for (y = chunk->starty; y <= chunk->endy; y++)
        {
            fragment = chunk->area->pixels + (y * chunk->area->params.width * chunk->area->params.antialias + x);
            if (fragment->flags.dirty)
            {
                FragmentCallback callback;
                Color col;

                callback = chunk->area->fragment_callbacks[fragment->flags.callback];
                if (callback.function)
                {
                    Vector3 location(fragment->data.location.x, fragment->data.location.y, fragment->data.location.z);
                    col = callback.function(chunk->area->renderer, location, callback.data);
                    /*colorNormalize(&col);*/
                }
                else
                {
                    col = COLOR_BLACK;
                }

                fragment->data.color.r = col.r;
                fragment->data.color.g = col.g;
                fragment->data.color.b = col.b;

                chunk->area->lock->acquire();
                fragment->flags.dirty = 0;
                _setDirtyPixel(chunk->area, x, y);
                chunk->area->lock->release();
            }
            chunk->area->pixel_done++;
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
void RenderArea::postProcess(int nbchunks)
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
    dx = params.width * params.antialias / nx;
    dy = params.height * params.antialias / ny;
    x = 0;
    y = 0;
    pixel_done = 0;

    for (i = 0; i < nbchunks; i++)
    {
        chunks[i].thread = NULL;
        chunks[i].area = this;
    }

    running = 0;
    loops = 0;
    while ((x < nx && !renderer->render_interrupt) || running > 0)
    {
        Thread::timeSleepMs(50);

        for (i = 0; i < nbchunks; i++)
        {
            if (chunks[i].thread)
            {
                if (chunks[i].finished)
                {
                    chunks[i].thread->join();
                    delete chunks[i].thread;
                    chunks[i].thread = NULL;
                    running--;
                }
                else if (renderer->render_interrupt)
                {
                    chunks[i].interrupt = 1;
                }
            }

            renderer->render_progress = 0.1 + ((double)pixel_done / (double)pixel_count) * 0.9;

            if (x < nx && !chunks[i].thread && !renderer->render_interrupt)
            {
                chunks[i].finished = 0;
                chunks[i].interrupt = 0;
                chunks[i].startx = x * dx;
                if (x == nx - 1)
                {
                    chunks[i].endx = params.width * params.antialias - 1;
                }
                else
                {
                    chunks[i].endx = (x + 1) * dx - 1;
                }
                chunks[i].starty = y * dy;
                if (y == ny - 1)
                {
                    chunks[i].endy = params.height * params.antialias - 1;
                }
                else
                {
                    chunks[i].endy = (y + 1) * dy - 1;
                }

                chunks[i].thread = new Thread(_renderPostProcessChunk);
                chunks[i].thread->start((void*)(chunks + i));
                running++;

                if (++y >= ny)
                {
                    x++;
                    y = 0;
                }
            }
        }

        if (++loops >= 10)
        {
            lock->acquire();
            processDirtyPixels();
            lock->release();

            loops = 0;
        }
    }

    processDirtyPixels();
    callback_update(1.0);
}

class RenderWriter:public PictureWriter
{
public:
    RenderWriter(RenderArea *area): area(area) {}

    virtual unsigned int getPixel(int x, int y) override
    {
        Color result = _getFinalPixel(area, x, y);
        result.normalize();
        return result.to32BitBGRA();
    }
private:
    RenderArea *area;
};

int RenderArea::saveToFile(const std::string &path)
{
    RenderWriter writer(this);
    return writer.save(path, params.width, params.height);
}

void RenderArea::setPreviewCallbacks(RenderCallbackStart start, RenderCallbackDraw draw, RenderCallbackUpdate update)
{
    callback_start = start ? start : _callbackStart;
    callback_draw = draw ? draw : _callbackDraw;
    callback_update = update ? update : _callbackUpdate;

    callback_start(params.width, params.height, background_color);

    setAllDirty();
    processDirtyPixels();

    callback_update(0.0);
}
