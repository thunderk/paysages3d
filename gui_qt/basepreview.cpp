#include "basepreview.h"

#include <math.h>
#include <QVector>
#include <QPainter>
#include <QTimer>
#include <QWheelEvent>
#include <QLabel>
#include "tools.h"
#include "../lib_paysages/system.h"

/*************** PreviewChunk ***************/
class PreviewChunk
{
public:

    PreviewChunk(BasePreview* preview, int xstart, int ystart, int xsize, int ysize)
    {
        _preview = preview;
        _xstart = xstart;
        _ystart = ystart;
        _xsize = xsize;
        _ysize = ysize;
        
        _need_render = true;
        _alive = true;
    }
    
    inline BasePreview* preview()
    {
        return _preview;
    }
    
    inline bool isOnFront()
    {
        return _preview->isVisible() && _preview->window()->isActiveWindow();
    }
    
    bool isFrom(BasePreview* preview)
    {
        return _preview == preview;
    }
    
    void interrupt()
    {
        _alive = false;
    }
    
    void update()
    {
        _need_render = true;
    }

    bool render()
    {
        bool changed = false;
        int revision;
        
        if (_need_render)
        {
            if (!isOnFront())
            {
                return false;
            }
            
            _need_render = false;
            
            QImage pixbuf = _preview->startChunkTransaction(_xstart, _ystart, _xsize, _ysize, &revision);

            for (int x = 0; x < _xsize; x++)
            {
                for (int y = 0; y < _ysize; y++)
                {
                    QRgb col = pixbuf.pixel(x, y);
                    if (!_alive)
                    {
                        return false;
                    }
                    if (qAlpha(col) < 255)
                    {
                        QColor newcol = _preview->getPixelColor(_xstart + x, _ystart + y);
                        newcol.setAlpha(255);
                        
                        pixbuf.setPixel(x, y, newcol.rgb());
                        
                        changed = true;
                    }
                }
            }
            
            if (changed)
            {
                _preview->commitChunkTransaction(&pixbuf, _xstart, _ystart, _xsize, _ysize, revision);
            }
        }
        
        return changed;
    }
private:
    BasePreview* _preview;
    bool _alive;
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
        QThread::usleep(5000);
    }
}

/*************** PreviewDrawingManager ***************/
PreviewDrawingManager::PreviewDrawingManager()
{
    _thread_count = systemGetCoreCount();
    _lastRendered = NULL;
}

void PreviewDrawingManager::startThreads()
{
    for (int i = 0; i < _thread_count; i++)
    {
        PreviewDrawingThread* thread = new PreviewDrawingThread();
        _threads.append(thread);
        thread->start();
    }
}

void PreviewDrawingManager::stopThreads()
{
    for (int i = 0; i < _threads.size(); i++)
    {
        _threads.at(i)->askStop();
    }
    for (int i = 0; i < _chunks.size(); i++)
    {
        _chunks.at(i)->interrupt();
    }
    for (int i = 0; i < _threads.size(); i++)
    {
        _threads.at(i)->wait();
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
    int removed = 0;
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
            
            removed++;
            i--;
        }
    }
    
    logDebug(QString("[Previews] %1 chunks removed, %2 remaining").arg(removed).arg(_chunks.size()));
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

void PreviewDrawingManager::updateAllChunks()
{
    for (int i = 0; i < _chunks.size(); i++)
    {
        PreviewChunk* chunk;
        chunk = _chunks.at(i);
        if (chunk->isOnFront())
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

    chunk = NULL;
    _lock.lock();
    if (!_updateQueue.isEmpty())
    {
        for (int i = _updateQueue.size(); i > 0; i--)
        {
            chunk = _updateQueue.takeFirst();
            if (chunk && i > 1 && chunk->preview() == _lastRendered)
            {
                _updateQueue.append(chunk);
            }
            else
            {
                break;
            }
        }
    }
    _lock.unlock();

    if (chunk)
    {
        _lastRendered = chunk->preview();
        chunk->render();
    }        
}

int PreviewDrawingManager::chunkCount()
{
    return _chunks.size();
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
    _revision = 0;
    
    _info = new QLabel(this);
    _info->setVisible(false);
    _info->setStyleSheet("QLabel { background-color: white; color: black; }");

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

    delete _info;
    delete pixbuf;
    delete lock_drawing;
}

void BasePreview::addOsd(QString name)
{
    _osd.append(PreviewOsd::getInstance(name));
    setMouseTracking(true);
}

void BasePreview::savePack(PackStream* stream)
{
    packWriteDouble(stream, &this->xoffset);
    packWriteDouble(stream, &this->yoffset);
    packWriteDouble(stream, &this->scaling);
}

void BasePreview::loadPack(PackStream* stream)
{
    packReadDouble(stream, &this->xoffset);
    packReadDouble(stream, &this->yoffset);
    packReadDouble(stream, &this->scaling);
    emit contentChange();
}

void BasePreview::initDrawers()
{
    _drawing_manager = new PreviewDrawingManager();
    _drawing_manager->startThreads();
}

void BasePreview::stopDrawers()
{
    _drawing_manager->stopThreads();
}

void BasePreview::reviveAll()
{
    logDebug("[Previews] Reviving all previews");
    _drawing_manager->updateAllChunks();
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

QImage BasePreview::startChunkTransaction(int x, int y, int w, int h, int* revision)
{
    *revision = _revision;
    return pixbuf->copy(x, y, w, h);
}

void BasePreview::commitChunkTransaction(QImage* chunk, int x, int y, int w, int h, int revision)
{
    lock_drawing->lock();
    if (revision == _revision)
    {
        for (int ix = 0; ix < w; ix++)
        {
            for (int iy = 0; iy < h; iy++)
            {
                pixbuf->setPixel(x + ix, y + iy, chunk->pixel(ix, iy));
            }
        }
        emit contentChange();
    }
    lock_drawing->unlock();
}

QColor BasePreview::getPixelColor(int x, int y)
{
    return getColor((double) (x - _width / 2) * scaling + xoffset, (double) (y - _height / 2) * scaling + yoffset);
}

void BasePreview::handleRedraw()
{
    lock_drawing->lock();

    updateData();
    invalidatePixbuf(128);

    lock_drawing->unlock();
}

void BasePreview::showEvent(QShowEvent* event)
{
    updateChunks();
}

void BasePreview::resizeEvent(QResizeEvent* event)
{
    QImage* image;
    int added = 0;

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
            _drawing_manager->addChunk(new PreviewChunk(this, x, y, x + 32 > _width ? _width - x : 32, y + 32 > _height ? _height - y : 32));
            added++;
        }
    }
    logDebug(QString("[Previews] %1 chunks added, %2 total").arg(added).arg(_drawing_manager->chunkCount()));

    delete image;

    this->lock_drawing->unlock();
}

void BasePreview::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.drawImage(0, 0, *this->pixbuf);
    
    QImage osd(pixbuf->size(), pixbuf->format());
    osd.fill(0x00000000);
    for (int i = 0; i < _osd.size(); i++)
    {
        _osd[i]->apply(&osd, xoffset, yoffset, scaling);
    }
    painter.drawImage(0, 0, osd);
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

void BasePreview::updateChunks()
{
    _drawing_manager->updateChunks(this);
    _revision++;
}

void BasePreview::invalidatePixbuf(int value)
{
    for (int ix = 0; ix < _width; ix++)
    {
        for (int iy = 0; iy < _height; iy++)
        {
            QRgb col = pixbuf->pixel(ix, iy);
            if (qAlpha(col) == 255)
            {
                pixbuf->setPixel(ix, iy, qRgba(qRed(col), qGreen(col), qBlue(col), value));
            }
        }
    }
    updateChunks();
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
    int width, height;
    
    width = this->width();
    height = this->height();

    if (event->buttons() & Qt::LeftButton)
    {
        int dx, dy;
        int ndx, ndy;
        
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
            if (ndx <= -width || ndx >= width || ndy <= -height || ndy >= height)
            {
                xoffset -= (double) ndx * scaling;
                yoffset -= (double) ndy * scaling;

                lock_drawing->lock();
                pixbuf->fill(0x00000000);
                updateChunks();
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
                pixbuf->fill(0x00000000);
                QPainter painter(pixbuf);
                painter.drawImage(xstart + ndx, ystart + ndy, part);

                updateChunks();
                lock_drawing->unlock();

                emit contentChange();
            }
        }

        mousex = event->x();
        mousey = event->y();
    }
    else
    {
        // Get text info from OSD
        bool found = false;
        for (int i = 0; i < _osd.size(); i++)
        {
            double x = xoffset + (event->x() - width / 2) * scaling;
            double y = yoffset + (event->y() - height / 2) * scaling;
            QString info = _osd[i]->getToolTip(x, y, scaling);
            if (not info.isEmpty())
            {
                _info->setText(info);
                _info->setVisible(true);
                found = true;
                break;
            }
        }
        if (not found)
        {
            _info->setVisible(false);
        }
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
        pixbuf->fill(0x00000000);
        QPainter painter(pixbuf);
        painter.drawImage(0, 0, part);
        invalidatePixbuf(254);
        lock_drawing->unlock();

        emit contentChange();
    }
    else if (scaling > old_scaling)
    {
        lock_drawing->lock();
        QImage part = pixbuf->scaled((int) floor(((double) width) * old_scaling / scaling), (int) floor(((double) height) * old_scaling / scaling), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        pixbuf->fill(0x00000000);
        QPainter painter(pixbuf);
        painter.drawImage((width - part.width()) / 2, (height - part.height()) / 2, part);
        invalidatePixbuf(254);
        lock_drawing->unlock();

        emit contentChange();
    }

    event->accept();
}

void BasePreview::leaveEvent(QEvent* event)
{
    _info->setVisible(false);
}
