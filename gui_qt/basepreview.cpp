#include "basepreview.h"

#include <math.h>
#include <QVector>
#include <QPainter>
#include <QTimer>
#include <QWheelEvent>

class PreviewChunk
{
public:

    PreviewChunk(BasePreview* preview, QImage* pixbuf, QMutex* lock, int xstart, int ystart, int xsize, int ysize)
    {
        _preview = preview;
        _pixbuf = pixbuf;
        _lock = lock;
        _xstart = xstart;
        _ystart = ystart;
        _xsize = xsize;
        _ysize = ysize;
        
        _need_render = true;
    }
    
    bool isFrom(BasePreview* preview)
    {
        return _preview == preview;
    }
    
    void update()
    {
        _need_render = true;
    }

    bool render()
    {
        bool changed = false;
        
        if (_need_render)
        {
            _need_render = false;
            
            _lock->lock();
            QImage tempbuffer = _pixbuf->copy(_xstart, _ystart, _xsize, _ysize);
            _lock->unlock();

            for (int x = 0; x < _xsize; x++)
            {
                for (int y = 0; y < _ysize; y++)
                {
                    QRgb col = tempbuffer.pixel(x, y);
                    if (qAlpha(col) < 255)
                    {
                        QColor newcol = _preview->getPixelColor(_xstart + x, _ystart + y);
                        newcol.setAlpha(255);
                        
                        tempbuffer.setPixel(x, y, newcol.rgb());
                        
                        changed = true;
                    }
                }
            }
            
            if (changed)
            {
                _lock->lock();
                for (int x = 0; x < _xsize; x++)
                {
                    for (int y = 0; y < _ysize; y++)
                    {
                        _pixbuf->setPixel(_xstart + x, _ystart + y, tempbuffer.pixel(x, y));
                    }
                }
                _lock->unlock();
                _preview->tellContentChange();
            }
        }
        
        return changed;
    }
private:
    BasePreview* _preview;
    QImage* _pixbuf;
    QMutex* _lock;
    bool _need_render;
    int _xstart;
    int _ystart;
    int _xsize;
    int _ysize;
};

static PreviewDrawingManager* _drawing_manager = NULL;

/*************** PreviewDrawingThread ***************/
PreviewDrawingThread::PreviewDrawingThread() :
QThread()
{
    _running = false;
}

void PreviewDrawingThread::askStop()
{
    _running = false;
}

void PreviewDrawingThread::run()
{
    _running = true;
    while (_running)
    {
        _drawing_manager->performOneThreadJob();
        QThread::usleep(50000);
    }
}

/*************** PreviewDrawingManager ***************/
PreviewDrawingManager::PreviewDrawingManager()
{
    _thread_count = QThread::idealThreadCount();
    if (_thread_count < 1)
    {
        _thread_count = 1;
    }
}

void PreviewDrawingManager::startThreads()
{
    for (int i = 0; i < _thread_count * 3; i++)
    {
        PreviewDrawingThread* thread = new PreviewDrawingThread();
        _threads.append(thread);
        thread->start();
    }
}

void PreviewDrawingManager::addChunk(PreviewChunk* chunk)
{
    _lock.lock();
    _chunks.append(chunk);
    _updateQueue.append(chunk);
    _lock.unlock();
}

void PreviewDrawingManager::removeChunks(BasePreview* preview)
{
    for (int i = 0; i < _chunks.size(); i++)
    {
        PreviewChunk* chunk;
        chunk = _chunks.at(i);
        if (chunk->isFrom(preview))
        {
            _lock.lock();
            _chunks.remove(i);
            _updateQueue.removeAll(chunk);
            _lock.unlock();
            
            i--;
        }
    }
}

void PreviewDrawingManager::updateChunks(BasePreview* preview)
{
    for (int i = 0; i < _chunks.size(); i++)
    {
        PreviewChunk* chunk;
        chunk = _chunks.at(i);
        if (chunk->isFrom(preview))
        {
            chunk->update();
            _lock.lock();
            if (!_updateQueue.contains(chunk))
            {   
                _updateQueue.prepend(chunk);
            }
            _lock.unlock();
        }
    }
}

void PreviewDrawingManager::performOneThreadJob()
{
    PreviewChunk* chunk;
    do
    {
        chunk = NULL;
        _lock.lock();
        if (!_updateQueue.isEmpty())
        {
            chunk = _updateQueue.takeFirst();
        }
        _lock.unlock();
        
        if (chunk)
        {
            chunk->render();
            PreviewDrawingThread::usleep(50000);
        }
        else
        {
            PreviewDrawingThread::usleep(50000);
        }
        
    } while (true);
}

/*************** BasePreview ***************/
BasePreview::BasePreview(QWidget* parent) :
QWidget(parent)
{
    this->lock_drawing = new QMutex();

    this->conf_scroll_xmin = 0.0;
    this->conf_scroll_xmax = 0.0;
    this->conf_scroll_xinit = 0.0;
    this->conf_scroll_ymin = 0.0;
    this->conf_scroll_ymax = 0.0;
    this->conf_scroll_yinit = 0.0;
    this->conf_scale_min = 1.0;
    this->conf_scale_max = 1.0;
    this->conf_scale_init = 1.0;
    this->conf_scale_step = 0.0;
    this->conf_scroll_logarithmic = false;
    this->scaling = 1.0;
    this->scalingbase = 1.0;
    this->xoffset = 0.0;
    this->yoffset = 0.0;
    this->pixbuf = new QImage(this->size(), QImage::Format_ARGB32);
    this->pixbuf->fill(0x00000000);
    _width = width();
    _height = height();

    this->alive = true;

    QObject::connect(this, SIGNAL(contentChange()), this, SLOT(update()));
    QObject::connect(this, SIGNAL(redrawRequested()), this, SLOT(handleRedraw()));

    this->setMinimumSize(256, 256);
    this->setMaximumSize(256, 256);
    this->resize(256, 256);
}

BasePreview::~BasePreview()
{
    alive = false;

    _drawing_manager->removeChunks(this);

    delete pixbuf;
    delete lock_drawing;
}

void BasePreview::initDrawers()
{
    _drawing_manager = new PreviewDrawingManager();
    _drawing_manager->startThreads();
}

void BasePreview::stopDrawers()
{
    //delete _drawing_manager;
}

void BasePreview::updateData()
{
}

QColor BasePreview::getColor(double x, double y)
{
    return QColor(0, 0, 0);
}

void BasePreview::configScaling(double min, double max, double step, double init, bool logarithmic)
{
    double size = (double) width();

    if (size >= 1.0)
    {
        conf_scale_min = min / size;
        conf_scale_max = max / size;
        conf_scale_step = step / size;
        conf_scale_init = init / size;
        conf_scroll_logarithmic = logarithmic;
        scalingbase = init / size;
        if (conf_scroll_logarithmic && conf_scale_max - conf_scale_min > 0.0000001)
        {
            scalingbase = pow((scalingbase - conf_scale_min) / (conf_scale_max - conf_scale_min), 0.5) * (conf_scale_max - conf_scale_min) + conf_scale_min;
        }

        updateScaling();
        redraw();
    }
}

void BasePreview::configScrolling(double xmin, double xmax, double xinit, double ymin, double ymax, double yinit)
{
    conf_scroll_xmin = xmin;
    conf_scroll_xmax = xmax;
    conf_scroll_xinit = xinit;
    conf_scroll_ymin = ymin;
    conf_scroll_ymax = ymax;
    conf_scroll_yinit = yinit;
    xoffset = xinit;
    yoffset = yinit;
    redraw();
}

void BasePreview::redraw()
{
    emit(redrawRequested());
}

QColor BasePreview::getPixelColor(int x, int y)
{
    return getColor((double) (x - _width / 2) * scaling + xoffset, (double) (y - _height / 2) * scaling + yoffset);
}

void BasePreview::handleRedraw()
{
    lock_drawing->lock();

    updateData();

    QImage part = pixbuf->copy();
    pixbuf->fill(0x00000000);
    QPainter painter(pixbuf);
    painter.setOpacity(0.99);
    painter.drawImage(0, 0, part);
    
    _drawing_manager->updateChunks(this);

    lock_drawing->unlock();
}

void BasePreview::resizeEvent(QResizeEvent* event)
{
    QImage* image;

    this->lock_drawing->lock();

    image = this->pixbuf;

    _width = event->size().width();
    _height = event->size().height();

    this->pixbuf = new QImage(this->size(), QImage::Format_ARGB32);

    this->pixbuf->fill(0x00000000);

    _drawing_manager->removeChunks(this);
    for (int x = 0; x < _width; x += 32)
    {
        for (int y = 0; y < _height; y += 32)
        {
            _drawing_manager->addChunk(new PreviewChunk(this, pixbuf, lock_drawing, x, y, x + 32 > _width ? _width - x : 32, y + 32 > _height ? _height - y : 32));
        }
    }

    delete image;

    this->lock_drawing->unlock();
}

void BasePreview::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.drawImage(0, 0, *this->pixbuf);
}

void BasePreview::updateScaling()
{
    if (conf_scroll_logarithmic && conf_scale_max - conf_scale_min > 0.0000001)
    {
        scaling = pow((scalingbase - conf_scale_min) / (conf_scale_max - conf_scale_min), 2.0) * (conf_scale_max - conf_scale_min) + conf_scale_min;
    }
    else
    {
        scaling = scalingbase;
    }
}

void BasePreview::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        mousex = event->x();
        mousey = event->y();
    }
}

void BasePreview::mouseMoveEvent(QMouseEvent* event)
{
    int dx, dy;
    int ndx, ndy;
    int width, height;

    if (event->buttons() & Qt::LeftButton)
    {
        dx = event->x() - mousex;
        dy = event->y() - mousey;

        ndx = dx;
        ndy = dy;
        if (xoffset - dx * scaling > conf_scroll_xmax)
        {
            ndx = (int) floor((conf_scroll_xmax - xoffset) / scaling);
        }
        if (xoffset - dx * scaling < conf_scroll_xmin)
        {
            ndx = (int) floor((conf_scroll_xmin - xoffset) / scaling);
        }
        if (yoffset - dy * scaling > conf_scroll_ymax)
        {
            ndy = (int) floor((conf_scroll_ymax - yoffset) / scaling);
        }
        if (yoffset - dy * scaling < conf_scroll_ymin)
        {
            ndy = (int) floor((conf_scroll_ymin - yoffset) / scaling);
        }
        if (ndx != 0 || ndy != 0)
        {
            width = this->width();
            height = this->height();

            if (ndx <= -width || ndx >= width || ndy <= -height || ndy >= height)
            {
                xoffset -= (double) ndx * scaling;
                yoffset -= (double) ndy * scaling;

                lock_drawing->lock();
                pixbuf->fill(0x00000000);
                _drawing_manager->updateChunks(this);
                lock_drawing->unlock();
            }
            else
            {
                int xstart, xsize, ystart, ysize;

                lock_drawing->lock();

                xoffset -= (double) ndx * scaling;
                yoffset -= (double) ndy * scaling;

                if (ndx < 0)
                {
                    xstart = -ndx;
                    xsize = width + ndx;
                }
                else
                {
                    xstart = 0;
                    xsize = width - ndx;
                }
                if (ndy < 0)
                {
                    ystart = -ndy;
                    ysize = height + ndy;
                }
                else
                {
                    ystart = 0;
                    ysize = height - ndy;
                }

                QImage part = pixbuf->copy(xstart, ystart, xsize, ysize);
                QPainter painter(pixbuf);
                pixbuf->fill(0x00000000);
                painter.drawImage(xstart + ndx, ystart + ndy, part);

                _drawing_manager->updateChunks(this);
                lock_drawing->unlock();

                emit contentChange();
            }
        }

        mousex = event->x();
        mousey = event->y();
    }
}

void BasePreview::wheelEvent(QWheelEvent* event)
{
    double factor;
    double old_scaling;
    int width, height;
    int new_width, new_height;

    if (event->modifiers() & Qt::ShiftModifier)
    {
        factor = 5.0;
    }
    else if (event->modifiers() & Qt::ControlModifier)
    {
        factor = 0.2;
    }
    else
    {
        factor = 1.0;
    }

    if (event->orientation() == Qt::Vertical)
    {
        if (event->delta() > 0 && scalingbase > conf_scale_min)
        {
            scalingbase -= factor * conf_scale_step;
            if (scalingbase < conf_scale_min)
            {
                scalingbase = conf_scale_min;
            }
        }
        else if (event->delta() < 0 && scalingbase < conf_scale_max)
        {
            scalingbase += factor * conf_scale_step;
            if (scalingbase > conf_scale_max)
            {
                scalingbase = conf_scale_max;
            }
        }
        else
        {
            event->ignore();
            return;
        }
    }
    else
    {
        event->ignore();
        return;
    }

    old_scaling = scaling;
    updateScaling();

    width = pixbuf->width();
    height = pixbuf->height();

    if (scaling < old_scaling)
    {
        lock_drawing->lock();
        new_width = (int) floor(((double) width) * scaling / old_scaling);
        new_height = (int) floor(((double) height) * scaling / old_scaling);
        QImage part = pixbuf->copy((width - new_width) / 2, (height - new_height) / 2, new_width, new_height).scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        QPainter painter(pixbuf);
        pixbuf->fill(0x00000000);
        painter.setOpacity(0.99);
        painter.drawImage(0, 0, part);
        _drawing_manager->updateChunks(this);
        lock_drawing->unlock();

        emit contentChange();
    }
    else if (scaling > old_scaling)
    {
        lock_drawing->lock();
        QImage part = pixbuf->scaled((int) floor(((double) width) * old_scaling / scaling), (int) floor(((double) height) * old_scaling / scaling), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        QPainter painter(pixbuf);
        pixbuf->fill(0x00000000);
        painter.setOpacity(0.99);
        painter.drawImage((width - part.width()) / 2, (height - part.height()) / 2, part);
        _drawing_manager->updateChunks(this);
        lock_drawing->unlock();

        emit contentChange();
    }

    event->accept();
}
