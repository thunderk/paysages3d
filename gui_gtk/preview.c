/* Small preview management */

#include <string.h>
#include <math.h>
#include "common.h"
#include "lib_paysages/shared/types.h"
#include "lib_paysages/shared/functions.h"
#include "lib_paysages/shared/constants.h"
#include "lib_paysages/shared/system.h"

#define MAX_PREVIEWS 30

struct SmallPreview
{
    double conf_scroll_xmin;
    double conf_scroll_xmax;
    double conf_scroll_ymin;
    double conf_scroll_ymax;

    double conf_scale_min;
    double conf_scale_max;
    double conf_scale_step;

    double xoffset;
    double yoffset;
    double scaling;

    Mutex* lock;
    GdkPixbuf* pixbuf;
    GtkImage* image;

    int mousex;
    int mousey;

    int need_rerender;
    int need_render;
    int need_update;
    SmallPreviewCallback renderer;
};

static int _previews_count = 0;
static SmallPreview _preview[MAX_PREVIEWS];
static Thread* _thread_update;

static inline void _forceRender(SmallPreview* preview)
{
    gdk_pixbuf_fill(preview->pixbuf, 0x00000000);
    preview->need_rerender = 0;
    preview->need_render = 1;
}

static inline void _updateImage(SmallPreview* preview)
{
    gtk_image_set_from_pixbuf(preview->image, preview->pixbuf);
}

static inline void _renderPixbuf(SmallPreview* preview)
{
    int x, y, done;
    Color col;
    void* pixels = gdk_pixbuf_get_pixels(preview->pixbuf);
    guint32* pixel;
    int rowstride = gdk_pixbuf_get_rowstride(preview->pixbuf);

    /* TODO Use pixbuf size */
    for (x = 0; x < 256; x++)
    {
        mutexAcquire(preview->lock);
        if (preview->need_rerender)
        {
            mutexRelease(preview->lock);
            break;
        }
        done = 0;
        for (y = 0; y < 256; y++)
        {
            pixel = (guint32*)(pixels + y * rowstride + x * 4);
            if (!*pixel)
            {
                col = preview->renderer(preview, (double)(x - 128) * preview->scaling + preview->xoffset, (double)(y - 128) * preview->scaling + preview->yoffset, preview->xoffset, preview->yoffset, preview->scaling);
                *pixel = (guint32)colorTo32BitRGBA(&col);
                done = 1;
            }
        }
        if (done)
        {
            preview->need_update = 1;
        }
        mutexRelease(preview->lock);
    }
}

static void* _doRenders(void* data)
{
    int i;
    SmallPreview* preview;

    while (1)
    {
        for (i = 0; i < _previews_count; i++)
        {
            preview = _preview + i;
            if (preview->need_rerender)
            {
                _forceRender(preview);
            }
            if (preview->need_render)
            {
                preview->need_render = 0;
                _renderPixbuf(preview);
            }
        }
        timeSleepMs(100);
    }
    return NULL;
}

static int _doUpdates(void* data)
{
    int i;
    SmallPreview* preview;

    for (i = 0; i < _previews_count; i++)
    {
        preview = _preview + i;
        if (preview->need_update)
        {
            preview->need_update = 0;
            _updateImage(preview);
        }
    }
    return 1;
}

static void _scrollPixbuf(SmallPreview* preview, int dx, int dy)
{
    int xstart, ystart, xsize, ysize, y;
    void* pixels = gdk_pixbuf_get_pixels(preview->pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(preview->pixbuf);

    preview->xoffset -= (double)dx * preview->scaling;
    preview->yoffset -= (double)dy * preview->scaling;

    /* TODO Use pixbuf size */
    if (dx == 0 && dy == 0)
    {
        return;
    }
    else if (dx <= -256 || dx >= 256 || dy <= -256 || dy >= 256)
    {
        _forceRender(preview);
    }
    else
    {
        if (dx < 0)
        {
            xstart = -dx;
            xsize = 256 + dx;
        }
        else
        {
            xstart = 0;
            xsize = 256 - dx;
        }
        if (dy < 0)
        {
            ystart = -dy;
            ysize = 256 + dy;
        }
        else
        {
            ystart = 0;
            ysize = 256 - dy;
        }
        memmove(pixels + (ystart + dy) * rowstride + (xstart + dx) * 4, pixels + ystart * rowstride + xstart * 4, (ysize - 1) * rowstride + xsize * 4);
        if (dy < 0)
        {
            memset(pixels + (256 + dy) * rowstride, 0, (-dy - 1) * rowstride + 256 * 4);
        }
        else if (dy > 0)
        {
            memset(pixels, 0, (dy - 1) * rowstride + 256 * 4);
        }
        if (dx < 0)
        {
            for (y = ystart + dy; y < ystart + dy + ysize; y++)
            {
                memset(pixels + y * rowstride + xsize * 4, 0, -dx * 4);
            }
        }
        else if (dx > 0)
        {
            for (y = ystart + dy; y < ystart + dy + ysize; y++)
            {
                memset(pixels + y * rowstride, 0, dx * 4);
            }
        }
        preview->need_render = 1;
    }
}

static inline int _fixScroll(SmallPreview* preview, int dx, int dy, int* new_dx, int* new_dy)
{
    *new_dx = dx;
    *new_dy = dy;
    if (preview->xoffset - dx * preview->scaling > preview->conf_scroll_xmax)
    {
        *new_dx = (int)floor((preview->conf_scroll_xmax - preview->xoffset) / preview->scaling);
    }
    if (preview->xoffset - dx * preview->scaling < preview->conf_scroll_xmin)
    {
        *new_dx = (int)floor((preview->conf_scroll_xmin - preview->xoffset) / preview->scaling);
    }
    if (preview->yoffset - dy * preview->scaling > preview->conf_scroll_ymax)
    {
        *new_dy = (int)floor((preview->conf_scroll_ymax - preview->yoffset) / preview->scaling);
    }
    if (preview->yoffset - dy * preview->scaling < preview->conf_scroll_ymin)
    {
        *new_dy = (int)floor((preview->conf_scroll_ymin - preview->yoffset) / preview->scaling);
    }
    return (*new_dx == 0 && *new_dy == 0) ? 0 : 1;
}

static inline int _fixScaling(SmallPreview* preview, double scaling, double* new_scaling)
{
    double old_scaling = preview->scaling;
    *new_scaling = scaling;
    if (scaling < preview->conf_scale_min)
    {
        *new_scaling = preview->conf_scale_min;
    }
    if (scaling > preview->conf_scale_max)
    {
        *new_scaling = preview->conf_scale_max;
    }
    return (old_scaling == *new_scaling) ? 0 : 1;
}

static int _cbMouseScroll(GtkEventBox* image, GdkEventScroll* event, gpointer data)
{
    SmallPreview* preview = (SmallPreview*)data;

    /* TODO Center the zoom on the cursor */

    if (event->direction == GDK_SCROLL_UP)
    {
        mutexAcquire(preview->lock);
        if (_fixScaling(preview, preview->scaling - preview->conf_scale_step, &preview->scaling))
        {
            preview->need_rerender = 1;
        }
        mutexRelease(preview->lock);
    }
    else if (event->direction == GDK_SCROLL_DOWN)
    {
        mutexAcquire(preview->lock);
        if (_fixScaling(preview, preview->scaling + preview->conf_scale_step, &preview->scaling))
        {
            preview->need_rerender = 1;
        }
        mutexRelease(preview->lock);
    }

    return 1;
}

static int _cbMouseButtonPressed(GtkEventBox* image, GdkEventButton* event, gpointer data)
{
    SmallPreview* preview = (SmallPreview*)data;

    if (event->button == 1)
    {
        preview->mousex = (int)event->x;
        preview->mousey = (int)event->y;
    }

    return 1;
}

static int _cbMouseMove(GtkEventBox* image, GdkEventMotion* event, gpointer data)
{
    SmallPreview* preview = (SmallPreview*)data;
    int dx, dy;

    if (event->state & GDK_BUTTON1_MASK)
    {
        mutexAcquire(preview->lock);

        dx = (int)event->x - preview->mousex;
        dy = (int)event->y - preview->mousey;

        if (_fixScroll(preview, dx, dy, &dx, &dy))
        {
            _scrollPixbuf(preview, dx, dy);
        }

        preview->mousex = (int)event->x;
        preview->mousey = (int)event->y;

        mutexRelease(preview->lock);
    }

    return 1;
}

static Color _renderNone(SmallPreview* preview, double x, double y, double xoffset, double yoffset, double scaling)
{
    return COLOR_BLACK;
}

void guiPreviewStart()
{
    _thread_update = threadCreate(_doRenders, NULL);
    g_timeout_add(200, _doUpdates, NULL);
}

void guiPreviewRedraw(SmallPreview* preview)
{
    mutexAcquire(preview->lock);
    preview->need_rerender = 1;
    mutexRelease(preview->lock);
}

void guiPreviewRedrawAll()
{
    int i;

    for (i = 0; i < _previews_count; i++)
    {
        guiPreviewRedraw(_preview + i);
    }
}

SmallPreview* guiPreviewNew(GtkImage* image)
{
    GtkWidget* parent = gtk_widget_get_parent(GTK_WIDGET(image));
    SmallPreview* preview;

    /* TODO Check the parent can receive events (or is a GtkEventBox) */
    if (_previews_count < MAX_PREVIEWS)
    {
        preview = _preview + _previews_count;
        preview->lock = mutexCreate();
        preview->conf_scroll_xmin = 0.0;
        preview->conf_scroll_xmax = 0.0;
        preview->conf_scroll_ymin = 0.0;
        preview->conf_scroll_ymax = 0.0;
        preview->conf_scale_min = 1.0;
        preview->conf_scale_max = 1.0;
        preview->conf_scale_step = 0.0;
        preview->scaling = 1.0;
        preview->xoffset = 0.0;
        preview->yoffset = 0.0;
        /* TODO Get size from GtkImage */
        preview->pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, 1, 8, 256, 256);
        preview->image = image;
        preview->need_rerender = 0;
        preview->need_render = 0;
        preview->need_update = 0;
        preview->renderer = _renderNone;

        gtk_image_clear(image);
        _forceRender(preview);

        g_signal_connect(parent, "scroll-event", G_CALLBACK(_cbMouseScroll), (gpointer)preview);
        g_signal_connect(parent, "button-press-event", G_CALLBACK(_cbMouseButtonPressed), (gpointer)preview);
        g_signal_connect(parent, "motion-notify-event", G_CALLBACK(_cbMouseMove), (gpointer)preview);

        _previews_count++;
        return preview;
    }
    else
    {
        /* TODO Return fake preview */
        return NULL;
    }
}

void guiPreviewConfigScrolling(SmallPreview* preview, double xmin, double xmax, double ymin, double ymax)
{
    mutexAcquire(preview->lock);
    preview->conf_scroll_xmin = xmin;
    preview->conf_scroll_xmax = xmax;
    preview->conf_scroll_ymin = ymin;
    preview->conf_scroll_ymax = ymax;
    preview->need_rerender = 1;
    mutexRelease(preview->lock);
}

void guiPreviewConfigScaling(SmallPreview* preview, double min, double max, double step)
{
    mutexAcquire(preview->lock);
    preview->conf_scale_min = min;
    preview->conf_scale_max = max;
    preview->conf_scale_step = step;
    preview->need_rerender = 1;
    mutexRelease(preview->lock);
}

void guiPreviewSetRenderer(SmallPreview* preview, SmallPreviewCallback renderer)
{
    mutexAcquire(preview->lock);
    preview->renderer = renderer;
    preview->need_rerender = 1;
    mutexRelease(preview->lock);
}

void guiPreviewSetViewport(SmallPreview* preview, double xoffset, double yoffset, double scaling)
{
    mutexAcquire(preview->lock);
    preview->xoffset = xoffset;
    preview->yoffset = yoffset;
    preview->scaling = scaling;
    preview->need_rerender = 1;
    mutexRelease(preview->lock);
}

void guiPreviewSetTerrainHeight(SmallPreview* preview)
{
    /*preview->conf_scroll_x = 1;
    preview->conf_scroll_y = 1;
    preview->conf_zoom = 1;
    preview->init_scaling = preview->scaling = 0.1;
    preview->init_xoffset = preview->xoffset = 0.0;
    preview->init_yoffset = preview->yoffset = 0.0;

    preview->renderer = _renderTopDownHeight;*/

    guiPreviewRedraw(preview);
}
