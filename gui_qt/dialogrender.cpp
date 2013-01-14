#include "dialogrender.h"

#include <math.h>
#include <QVBoxLayout>
#include <QImage>
#include <QColor>
#include <QPainter>
#include <QMessageBox>
#include "tools.h"

#include "../lib_paysages/scenery.h"
#include "../lib_paysages/auto.h"

static DialogRender* _current_dialog;

static void _renderStart(int width, int height, Color background)
{
    _current_dialog->pixbuf_lock->lock();
    delete _current_dialog->pixbuf;
    _current_dialog->pixbuf = new QImage(width, height, QImage::Format_ARGB32);
    _current_dialog->pixbuf->fill(colorToQColor(background).rgb());
    _current_dialog->pixbuf_lock->unlock();

    _current_dialog->tellRenderSize(width, height);
}

static void _renderDraw(int x, int y, Color col)
{
    col = colorProfileApply(_current_dialog->_hdr_profile, col);
    _current_dialog->pixbuf->setPixel(x, _current_dialog->pixbuf->height() - 1 - y, colorToQColor(col).rgb());
}

static void _renderUpdate(double progress)
{
    _current_dialog->area->update();
    _current_dialog->tellProgressChange(progress);
}

class RenderThread:public QThread
{
public:
    RenderThread(Renderer* renderer, RenderParams params):QThread()
    {
        _renderer = renderer;
        _params = params;
    }
    void run()
    {
        rendererStart(_renderer, _params);
    }
private:
    Renderer* _renderer;
    RenderParams _params;
};

class RenderArea:public QWidget
{
public:
    RenderArea(QWidget* parent):
        QWidget(parent)
    {
        setMinimumSize(800, 600);
    }

    void paintEvent(QPaintEvent*)
    {
        QPainter painter(this);
        _current_dialog->pixbuf_lock->lock();
        painter.drawImage(0, 0, *_current_dialog->pixbuf);
        _current_dialog->pixbuf_lock->unlock();
    }
};

DialogRender::DialogRender(QWidget *parent, Renderer* renderer):
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint)
{
    pixbuf_lock = new QMutex();
    pixbuf = new QImage(1, 1, QImage::Format_ARGB32);
    _current_dialog = this;
    _render_thread = NULL;
    _renderer = renderer;

    _hdr_profile = colorProfileCreate();

    setModal(true);
    setWindowTitle(tr("Paysages 3D - Render"));
    setLayout(new QVBoxLayout());

    _scroll = new QScrollArea(this);
    _scroll->setAlignment(Qt::AlignCenter);
    area = new RenderArea(_scroll);
    _scroll->setWidget(area);
    layout()->addWidget(_scroll);

    _info = new QWidget(this);
    _info->setLayout(new QHBoxLayout());
    layout()->addWidget(_info);

    _timer = new QLabel(QString("0:00.00"), _info);
    _info->layout()->addWidget(_timer);

    _progress = new QProgressBar(_info);
    _progress->setMaximumHeight(12);
    _progress->setMinimum(0);
    _progress->setMaximum(1000);
    _progress->setValue(0);
    _info->layout()->addWidget(_progress);

    connect(this, SIGNAL(renderSizeChanged(int, int)), this, SLOT(applyRenderSize(int, int)));
    connect(this, SIGNAL(progressChanged(double)), this, SLOT(applyProgress(double)));
}

DialogRender::~DialogRender()
{
    if (_render_thread)
    {
        rendererInterrupt(_renderer);
        _render_thread->wait();

        delete _render_thread;
    }
    colorProfileDelete(_hdr_profile);
    delete pixbuf;
    delete pixbuf_lock;
}

void DialogRender::tellRenderSize(int width, int height)
{
    emit renderSizeChanged(width, height);
}

void DialogRender::tellProgressChange(double value)
{
    emit progressChanged(value);
}

void DialogRender::startRender(RenderParams params)
{
    _started = time(NULL);

    //applyRenderSize(params.width, params.height);
    rendererSetPreviewCallbacks(_renderer, _renderStart, _renderDraw, _renderUpdate);

    _render_thread = new RenderThread(_renderer, params);
    _render_thread->start();

    exec();
}

void DialogRender::loadLastRender()
{
    //applyRenderSize(_renderer->render_width, _renderer->render_height);
    _info->hide();
    rendererSetPreviewCallbacks(_renderer, _renderStart, _renderDraw, _renderUpdate);

    exec();
}

void DialogRender::applyRenderSize(int width, int height)
{
    area->setMinimumSize(width, height);
    area->setMaximumSize(width, height);
    area->resize(width, height);
    _scroll->setMinimumSize(width > 800 ? 820 : width + 20, height > 600 ? 620 : height + 20);
}

void DialogRender::applyProgress(double value)
{
    double diff = difftime(time(NULL), _started);
    int hours = (int)floor(diff / 3600.0);
    int minutes = (int)floor((diff - 3600.0 * hours) / 60.0);
    int seconds = (int)floor(diff - 3600.0 * hours - 60.0 * minutes);
    _timer->setText(tr("%1:%2.%3").arg(hours).arg(minutes, 2, 10, QLatin1Char('0')).arg(seconds, 2, 10, QLatin1Char('0')));
    _progress->setValue((int)(value * 1000.0));
    _progress->update();
}

