#include "preview.h"
#include <QVector>
#include <QPainter>
#include <QThread>
#include <QTimer>

static QVector<Preview*> _previews;

class PreviewDrawer:public QThread
{
public:
    PreviewDrawer()
    {
    }
protected:
    void run()
    {
        while (true)
        {
            QVectorIterator<Preview*> iter(_previews);
            while (iter.hasNext())
            {
                iter.next()->doRender();
            }
            QThread::usleep(50000);
        }
    }
};

Preview::Preview(QWidget* parent) :
    QWidget(parent)
{
    _previews.append(this);

    this->lock = new QMutex();
    this->conf_scroll_xmin = 0.0;
    this->conf_scroll_xmax = 0.0;
    this->conf_scroll_ymin = 0.0;
    this->conf_scroll_ymax = 0.0;
    this->conf_scale_min = 1.0;
    this->conf_scale_max = 1.0;
    this->conf_scale_step = 0.0;
    this->scaling = 1.0;
    this->xoffset = 0.0;
    this->yoffset = 0.0;
    this->pixbuf = new QImage(this->size(), QImage::Format_ARGB32);
    this->need_rerender = 0;
    this->need_render = 0;

    this->resize(parent->size());
}

void Preview::startUpdater()
{
    (new PreviewDrawer())->start();
}

void Preview::doRender()
{
    if (this->need_rerender)
    {
        this->forceRender();
    }
    if (this->need_render)
    {
        this->need_render = 0;
        this->renderPixbuf();
    }
}

void Preview::redraw()
{
    lock->lock();
    need_rerender = 1;
    lock->unlock();
}

void Preview::resizeEvent(QResizeEvent* event)
{
    QImage* image;

    this->lock->lock();
    image = this->pixbuf;
    this->pixbuf = new QImage(this->size(), QImage::Format_ARGB32);
    delete image;
    this->lock->unlock();

    this->forceRender();
}

void Preview::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.drawImage(0, 0, *this->pixbuf);
}

void Preview::forceRender()
{
    this->lock->lock();
    this->pixbuf->fill(0x00000000);
    this->need_rerender = 0;
    this->need_render = 1;
    this->lock->unlock();
}

void Preview::renderPixbuf()
{
    QColor col;
    bool done;
    int x, y, w, h;

    w = this->pixbuf->width();
    h = this->pixbuf->height();

    for (x = 0; x < w; x++)
    {
        this->lock->lock();

        if (this->need_rerender)
        {
            this->lock->unlock();
            break;
        }

        done = false;
        for (y = 0; y < h; y++)
        {
            if (qAlpha(this->pixbuf->pixel(x, y)) == 0)
            {
                col = this->getColor((double)(x - w / 2) * this->scaling + this->xoffset, (double)(y - h / 2) * this->scaling + this->yoffset);
                this->pixbuf->setPixel(x, y, col.rgb());
                done = true;
            }
        }
        if (done)
        {
            this->update();
        }

        this->lock->unlock();
    }
}


//static void _scrollPixbuf(SmallPreview* preview, int dx, int dy)
//{
//    int xstart, ystart, xsize, ysize, y;
//    void* pixels = gdk_pixbuf_get_pixels(preview->pixbuf);
//    int rowstride = gdk_pixbuf_get_rowstride(preview->pixbuf);

//    preview->xoffset -= (double)dx * preview->scaling;
//    preview->yoffset -= (double)dy * preview->scaling;

//    /* TODO Use pixbuf size */
//    if (dx == 0 && dy == 0)
//    {
//        return;
//    }
//    else if (dx <= -256 || dx >= 256 || dy <= -256 || dy >= 256)
//    {
//        _forceRender(preview);
//    }
//    else
//    {
//        if (dx < 0)
//        {
//            xstart = -dx;
//            xsize = 256 + dx;
//        }
//        else
//        {
//            xstart = 0;
//            xsize = 256 - dx;
//        }
//        if (dy < 0)
//        {
//            ystart = -dy;
//            ysize = 256 + dy;
//        }
//        else
//        {
//            ystart = 0;
//            ysize = 256 - dy;
//        }
//        memmove(pixels + (ystart + dy) * rowstride + (xstart + dx) * 4, pixels + ystart * rowstride + xstart * 4, (ysize - 1) * rowstride + xsize * 4);
//        if (dy < 0)
//        {
//            memset(pixels + (256 + dy) * rowstride, 0, (-dy - 1) * rowstride + 256 * 4);
//        }
//        else if (dy > 0)
//        {
//            memset(pixels, 0, (dy - 1) * rowstride + 256 * 4);
//        }
//        if (dx < 0)
//        {
//            for (y = ystart + dy; y < ystart + dy + ysize; y++)
//            {
//                memset(pixels + y * rowstride + xsize * 4, 0, -dx * 4);
//            }
//        }
//        else if (dx > 0)
//        {
//            for (y = ystart + dy; y < ystart + dy + ysize; y++)
//            {
//                memset(pixels + y * rowstride, 0, dx * 4);
//            }
//        }
//        preview->need_render = 1;
//    }
//}

//static inline int _fixScroll(SmallPreview* preview, int dx, int dy, int* new_dx, int* new_dy)
//{
//    *new_dx = dx;
//    *new_dy = dy;
//    if (preview->xoffset - dx * preview->scaling > preview->conf_scroll_xmax)
//    {
//        *new_dx = (int)floor((preview->conf_scroll_xmax - preview->xoffset) / preview->scaling);
//    }
//    if (preview->xoffset - dx * preview->scaling < preview->conf_scroll_xmin)
//    {
//        *new_dx = (int)floor((preview->conf_scroll_xmin - preview->xoffset) / preview->scaling);
//    }
//    if (preview->yoffset - dy * preview->scaling > preview->conf_scroll_ymax)
//    {
//        *new_dy = (int)floor((preview->conf_scroll_ymax - preview->yoffset) / preview->scaling);
//    }
//    if (preview->yoffset - dy * preview->scaling < preview->conf_scroll_ymin)
//    {
//        *new_dy = (int)floor((preview->conf_scroll_ymin - preview->yoffset) / preview->scaling);
//    }
//    return (*new_dx == 0 && *new_dy == 0) ? 0 : 1;
//}

//static inline int _fixScaling(SmallPreview* preview, double scaling, double* new_scaling)
//{
//    double old_scaling = preview->scaling;
//    *new_scaling = scaling;
//    if (scaling < preview->conf_scale_min)
//    {
//        *new_scaling = preview->conf_scale_min;
//    }
//    if (scaling > preview->conf_scale_max)
//    {
//        *new_scaling = preview->conf_scale_max;
//    }
//    return (old_scaling == *new_scaling) ? 0 : 1;
//}

//static int _cbMouseScroll(GtkEventBox* image, GdkEventScroll* event, gpointer data)
//{
//    SmallPreview* preview = (SmallPreview*)data;

//    /* TODO Center the zoom on the cursor */

//    if (event->direction == GDK_SCROLL_UP)
//    {
//        mutexAcquire(preview->lock);
//        if (_fixScaling(preview, preview->scaling - preview->conf_scale_step, &preview->scaling))
//        {
//            preview->need_rerender = 1;
//        }
//        mutexRelease(preview->lock);
//    }
//    else if (event->direction == GDK_SCROLL_DOWN)
//    {
//        mutexAcquire(preview->lock);
//        if (_fixScaling(preview, preview->scaling + preview->conf_scale_step, &preview->scaling))
//        {
//            preview->need_rerender = 1;
//        }
//        mutexRelease(preview->lock);
//    }

//    return 1;
//}

//static int _cbMouseButtonPressed(GtkEventBox* image, GdkEventButton* event, gpointer data)
//{
//    SmallPreview* preview = (SmallPreview*)data;

//    if (event->button == 1)
//    {
//        preview->mousex = (int)event->x;
//        preview->mousey = (int)event->y;
//    }

//    return 1;
//}

//static int _cbMouseMove(GtkEventBox* image, GdkEventMotion* event, gpointer data)
//{
//    SmallPreview* preview = (SmallPreview*)data;
//    int dx, dy;

//    if (event->state & GDK_BUTTON1_MASK)
//    {
//        mutexAcquire(preview->lock);

//        dx = (int)event->x - preview->mousex;
//        dy = (int)event->y - preview->mousey;

//        if (_fixScroll(preview, dx, dy, &dx, &dy))
//        {
//            _scrollPixbuf(preview, dx, dy);
//        }

//        preview->mousex = (int)event->x;
//        preview->mousey = (int)event->y;

//        mutexRelease(preview->lock);
//    }

//    return 1;
//}

//void guiPreviewRedrawAll()
//{
//    int i;

//    for (i = 0; i < _previews_count; i++)
//    {
//        guiPreviewRedraw(_preview + i);
//    }
//}

//void guiPreviewConfigScrolling(SmallPreview* preview, double xmin, double xmax, double ymin, double ymax)
//{
//    mutexAcquire(preview->lock);
//    preview->conf_scroll_xmin = xmin;
//    preview->conf_scroll_xmax = xmax;
//    preview->conf_scroll_ymin = ymin;
//    preview->conf_scroll_ymax = ymax;
//    preview->need_rerender = 1;
//    mutexRelease(preview->lock);
//}

//void guiPreviewConfigScaling(SmallPreview* preview, double min, double max, double step)
//{
//    mutexAcquire(preview->lock);
//    preview->conf_scale_min = min;
//    preview->conf_scale_max = max;
//    preview->conf_scale_step = step;
//    preview->need_rerender = 1;
//    mutexRelease(preview->lock);
//}

//void guiPreviewSetRenderer(SmallPreview* preview, SmallPreviewCallback renderer)
//{
//    mutexAcquire(preview->lock);
//    preview->renderer = renderer;
//    preview->need_rerender = 1;
//    mutexRelease(preview->lock);
//}

//void guiPreviewSetViewport(SmallPreview* preview, double xoffset, double yoffset, double scaling)
//{
//    mutexAcquire(preview->lock);
//    preview->xoffset = xoffset;
//    preview->yoffset = yoffset;
//    preview->scaling = scaling;
//    preview->need_rerender = 1;
//    mutexRelease(preview->lock);
//}

//void guiPreviewSetTerrainHeight(SmallPreview* preview)
//{
//    /*preview->conf_scroll_x = 1;
//    preview->conf_scroll_y = 1;
//    preview->conf_zoom = 1;
//    preview->init_scaling = preview->scaling = 0.1;
//    preview->init_xoffset = preview->xoffset = 0.0;
//    preview->init_yoffset = preview->yoffset = 0.0;

//    preview->renderer = _renderTopDownHeight;*/

//    guiPreviewRedraw(preview);
//}
