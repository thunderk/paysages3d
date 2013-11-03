#include "basepreview.h"

#include <QVector>
#include <QPainter>
#include <QTimer>
#include <QWheelEvent>
#include <QLabel>
#include <QMenu>
#include <assert.h>
#include <math.h>
#include "tools.h"
#include "System.h"

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
        _rendering = false;
        _alive = true;
        _priority = xstart;
    }

    inline BasePreview* preview()
    {
        return _preview;
    }

    inline int priority()
    {
        return _priority;
    }

    inline bool isOnFront()
    {
        return _preview->isVisible();// && _preview->window()->isActiveWindow();
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
        _priority = _xstart;
        _need_render = true;
    }

    bool render()
    {
        bool changed = false;
        int revision;

        if (_rendering)
        {
            return false;
        }

        _rendering = true;
        _alive = true;

        if (_need_render)
        {
            if (!isOnFront())
            {
                _priority = -1;
                _rendering = false;
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
                        _need_render = true;
                        _preview->rollbackChunkTransaction();
                        _rendering = false;
                        return true;
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
                _need_render = not _preview->commitChunkTransaction(&pixbuf, _xstart, _ystart, _xsize, _ysize, revision);
            }
            else
            {
                _preview->rollbackChunkTransaction();
            }
        }

        if (not _need_render)
        {
            _priority = -1;
        }

        _rendering = false;
        return _need_render;
    }
private:
    BasePreview* _preview;
    int _priority;
    bool _alive;
    bool _need_render;
    bool _rendering;
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
    _thread_count = System::getCoreCount();
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
    logDebug(QString("[Previews] Stopping all render threads"));
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

void PreviewDrawingManager::suspendChunks(BasePreview* preview)
{
    _lock.lock();
    for (int i = 0; i < _updateQueue.size(); i++)
    {
        PreviewChunk* chunk;
        chunk = _updateQueue.at(i);
        if (chunk->isFrom(preview))
        {
            _updateQueue.takeAt(i);
            i--;
        }
    }
    for (int i = 0; i < _chunks.size(); i++)
    {
        _chunks.at(i)->interrupt();
    }
    _lock.unlock();
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
    logDebug(QString("[Previews] Reviving all %1 preview chunks").arg(_chunks.size()));
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

static bool _cmpChunkPriority(const PreviewChunk* chunk1, const PreviewChunk* chunk2)
{
    return ((PreviewChunk*)chunk1)->priority() < ((PreviewChunk*)chunk2)->priority();
}


void PreviewDrawingManager::performOneThreadJob()
{
    PreviewChunk* chunk;

    chunk = NULL;
    _lock.lock();
    if (!_updateQueue.isEmpty())
    {
        qSort(_updateQueue.begin(), _updateQueue.end(), _cmpChunkPriority);
        chunk = _updateQueue.takeFirst();
    }
    _lock.unlock();

    if (chunk)
    {
        if (chunk->render())
        {
            _lock.lock();
            if (!_updateQueue.contains(chunk))
            {
                _updateQueue.append(chunk);
            }
            _lock.unlock();
        }
    }
}

int PreviewDrawingManager::chunkCount()
{
    return _chunks.size();
}

/*************** BasePreview ***************/
BasePreview::BasePreview(QWidget* parent) :
DrawingWidget(parent)
{
    this->_lock_drawing = new QMutex();

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
    this->_pixbuf = new QImage(this->size(), QImage::Format_ARGB32);
    this->_pixbuf->fill(0x00000000);
    _width = width();
    _height = height();
    _revision = 0;
    _transactions_count = 0;
    _redraw_requested = false;

    _renderer = NULL;

    _info = new QLabel(this);
    _info->setVisible(false);
    _info->setStyleSheet("QLabel { background-color: white; color: black; }");

    _hdr_enabled = false;
    _hdr_profile = colorProfileCreate();

    this->alive = true;

    QObject::connect(this, SIGNAL(contentChange()), this, SLOT(update()));

    this->setMinimumSize(256, 256);
    this->setMaximumSize(256, 256);
    this->resize(256, 256);

    startTimer(50);
}

BasePreview::~BasePreview()
{
    alive = false;

    colorProfileDelete(_hdr_profile);

    _drawing_manager->removeChunks(this);

    delete _info;
    delete _pixbuf;
    delete _lock_drawing;
}

void BasePreview::setRenderer(PreviewRenderer* renderer)
{
    _renderer = renderer;
    _renderer->bindEvent(this);
}

void BasePreview::configHdrToneMapping(bool active)
{
    _hdr_enabled = active;
    redraw();
}

void BasePreview::addOsd(QString name)
{
    _osd.append(PreviewOsd::getInstance(name));
    setMouseTracking(true);
}

void BasePreview::savePack(PackStream* stream)
{
    stream->write(&this->xoffset);
    stream->write(&this->yoffset);
    stream->write(&this->scalingbase);

    // TODO Save choices and toggles
}

void BasePreview::loadPack(PackStream* stream)
{
    stream->read(&this->xoffset);
    stream->read(&this->yoffset);
    stream->read(&this->scalingbase);

    // TODO Save choices and toggles

    updateScaling();
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
    _drawing_manager->updateAllChunks();
}

void BasePreview::updateData()
{
    if (_renderer)
    {
        _renderer->updateEvent();
    }
}

void BasePreview::cameraEvent()
{
}

Color BasePreview::getColor(double x, double y)
{
    if (_renderer)
    {
        return _renderer->getColor2D(x, y, scaling);
    }
    else
    {
        return COLOR_BLACK;
    }
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

void BasePreview::addChoice(const QString& key, const QString& title, const QStringList& choices, int init_value)
{
    _ContextChoice choice;
    choice.title = title;
    choice.items << choices;
    choice.current = init_value;

    _choices.insert(key, choice);

    choiceChangeEvent(key, init_value);
}

void BasePreview::choiceChangeEvent(const QString&, int)
{
}

void BasePreview::addToggle(const QString& key, const QString& text, bool init_value)
{
    _ContextToggle toggle;
    toggle.title = text;
    toggle.value = init_value;

    _toggles.insert(key, toggle);

    toggleChangeEvent(key, init_value);
}

void BasePreview::toggleChangeEvent(QString, bool)
{
    redraw();
}

void BasePreview::redraw()
{
    _drawing_manager->suspendChunks(this);
    _redraw_requested = true;
}

QImage BasePreview::startChunkTransaction(int x, int y, int w, int h, int* revision)
{
    QImage result;

    _lock_drawing->lock();

    *revision = _revision;
    result = _pixbuf->copy(x, y, w, h);
    _transactions_count++;

    _lock_drawing->unlock();

    return result;
}

bool BasePreview::commitChunkTransaction(QImage* chunk, int x, int y, int w, int h, int revision)
{
    bool result;

    _lock_drawing->lock();

    if (revision == _revision)
    {
        for (int ix = 0; ix < w; ix++)
        {
            for (int iy = 0; iy < h; iy++)
            {
                _pixbuf->setPixel(x + ix, y + iy, chunk->pixel(ix, iy));
            }
        }
        emit contentChange();
        result = true;
    }
    else
    {
        result = false;
    }

    _transactions_count--;

    _lock_drawing->unlock();

    return result;
}

void BasePreview::rollbackChunkTransaction()
{
    _lock_drawing->lock();

    _transactions_count--;

    _lock_drawing->unlock();
}

QColor BasePreview::getPixelColor(int x, int y)
{
    Color col = getColor((double) (x - _width / 2) * scaling + xoffset, (double) (y - _height / 2) * scaling + yoffset);
    if (_hdr_enabled)
    {
        col = colorProfileApply(_hdr_profile, col);
    }
    else
    {
        colorNormalize(&col);
    }
    return colorToQColor(col);
}

void BasePreview::timerEvent(QTimerEvent*)
{
    _lock_drawing->lock();

    if (_redraw_requested && _transactions_count == 0)
    {
        updateData();
        invalidatePixbuf(128);
        _redraw_requested = false;
    }

    _lock_drawing->unlock();
}

void BasePreview::choiceSelected(QAction* action)
{
    switch (action->property("mode").toInt())
    {
    case 1:
    {
        QString key = action->property("key").toString();
        int value = action->property("value").toInt();

        _choices[key].current = value;

        choiceChangeEvent(key, value);
        break;
    }
    case 2:
    {
        QString key = action->property("key").toString();
        bool value = action->property("value").toBool();

        _toggles[key].value = value;

        toggleChangeEvent(key, value);
        break;
    }
    default:
        ;
    }
}

void BasePreview::showEvent(QShowEvent*)
{
    updateChunks();
}

void BasePreview::resizeEvent(QResizeEvent* event)
{
    QImage* image;
    int added = 0;

    this->_lock_drawing->lock();

    image = this->_pixbuf;

    _width = event->size().width();
    _height = event->size().height();

    this->_pixbuf = new QImage(this->size(), QImage::Format_ARGB32);

    this->_pixbuf->fill(0x00000000);

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

    this->_lock_drawing->unlock();
}

void BasePreview::doDrawing(QPainter* painter)
{
    if (isEnabled())
    {
        painter->drawImage(0, 0, *this->_pixbuf);

        QImage osd(_pixbuf->size(), _pixbuf->format());
        osd.fill(0x00000000);
        for (int i = 0; i < _osd.size(); i++)
        {
            _osd[i]->apply(&osd, xoffset, yoffset, scaling);
        }
        painter->drawImage(0, 0, osd);
    }
    else
    {
        painter->fillRect(rect(), QColor(100, 100, 100));
    }
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
    cameraEvent();
    _drawing_manager->updateChunks(this);
    _revision++;
}

void BasePreview::invalidatePixbuf(int value)
{
    for (int ix = 0; ix < _width; ix++)
    {
        for (int iy = 0; iy < _height; iy++)
        {
            QRgb col = _pixbuf->pixel(ix, iy);
            if (qAlpha(col) == 255)
            {
                _pixbuf->setPixel(ix, iy, qRgba(qRed(col), qGreen(col), qBlue(col), value));
            }
        }
    }
    updateChunks();
}

void BasePreview::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu(this);

    // TODO Get menu items from OSDs

    // Fill with choices
    QHashIterator<QString, _ContextChoice> iter1(_choices);
    while (iter1.hasNext())
    {
        if (not menu.isEmpty())
        {
            menu.addSeparator();
        }

        iter1.next();
        menu.addAction(QString("   %1   ").arg(iter1.value().title))->setDisabled(true);

        QStringList list = iter1.value().items;
        for (int i = 0; i < list.count(); i++)
        {
            QAction* action = menu.addAction(list[i]);
            action->setProperty("mode", 1);
            action->setProperty("key", iter1.key());
            action->setProperty("value", i);
            if (i == iter1.value().current)
            {
                action->setIcon(QIcon(getDataPath("images/choice_on.png")));
                action->setIconVisibleInMenu(true);
            }
        }
    }

    // Fill with toggles
    QHashIterator<QString, _ContextToggle> iter2(_toggles);
    while (iter2.hasNext())
    {
        if (not menu.isEmpty() and not iter2.hasPrevious())
        {
            menu.addSeparator();
        }

        iter2.next();

        QAction* action = menu.addAction(iter2.value().title);
        action->setProperty("mode", 2);
        action->setProperty("key", iter2.key());
        action->setProperty("value", not iter2.value().value);
        if (iter2.value().value)
        {
            action->setIcon(QIcon(getDataPath("images/toggle_on.png")));
            action->setIconVisibleInMenu(true);
        }
    }

    if (not menu.isEmpty())
    {
        connect(&menu, SIGNAL(triggered(QAction*)), this, SLOT(choiceSelected(QAction*)));

        menu.exec(event->globalPos());
        event->accept();
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

                _lock_drawing->lock();
                _pixbuf->fill(0x00000000);
                updateChunks();
                _lock_drawing->unlock();
            }
            else
            {
                int xstart, xsize, ystart, ysize;

                _lock_drawing->lock();

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

                QImage part = _pixbuf->copy(xstart, ystart, xsize, ysize);
                _pixbuf->fill(0x00000000);
                QPainter painter(_pixbuf);
                painter.drawImage(xstart + ndx, ystart + ndy, part);

                updateChunks();
                _lock_drawing->unlock();

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

    width = _pixbuf->width();
    height = _pixbuf->height();

    if (scaling < old_scaling)
    {
        _lock_drawing->lock();
        new_width = (int) floor(((double) width) * scaling / old_scaling);
        new_height = (int) floor(((double) height) * scaling / old_scaling);
        QImage part = _pixbuf->copy((width - new_width) / 2, (height - new_height) / 2, new_width, new_height).scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        _pixbuf->fill(0x00000000);
        QPainter painter(_pixbuf);
        painter.drawImage(0, 0, part);
        invalidatePixbuf(254);
        _lock_drawing->unlock();

        emit contentChange();
    }
    else if (scaling > old_scaling)
    {
        _lock_drawing->lock();
        QImage part = _pixbuf->scaled((int) floor(((double) width) * old_scaling / scaling), (int) floor(((double) height) * old_scaling / scaling), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        _pixbuf->fill(0x00000000);
        QPainter painter(_pixbuf);
        painter.drawImage((width - part.width()) / 2, (height - part.height()) / 2, part);
        invalidatePixbuf(254);
        _lock_drawing->unlock();

        emit contentChange();
    }

    event->accept();
}

void BasePreview::leaveEvent(QEvent*)
{
    _info->setVisible(false);
}
