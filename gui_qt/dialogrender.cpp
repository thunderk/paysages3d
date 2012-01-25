#include "dialogrender.h"

#include <QVBoxLayout>
#include <QImage>
#include <QColor>
#include <QPainter>

#include "../lib_paysages/render.h"
#include "../lib_paysages/scenery.h"
#include "../lib_paysages/auto.h"
#include "../lib_paysages/shared/functions.h"

class RenderThread:public QThread
{
public:
    RenderThread(Renderer* renderer):QThread()
    {
        _renderer = renderer;
    }
    void run()
    {
        autoRenderSceneTwoPass(_renderer, 0);
    }
private:
    Renderer* _renderer;
};

static DialogRender* _current_dialog;

static void _renderResize(int width, int height)
{
    delete _current_dialog->pixbuf;
    _current_dialog->pixbuf = new QImage(width, height, QImage::Format_ARGB32);
    _current_dialog->area->setMinimumSize(width, height);
    _current_dialog->area->setMaximumSize(width, height);
    _current_dialog->area->resize(width, height);
    _current_dialog->scroll->setMinimumSize(width > 800 ? 850 : width + 50, height > 600 ? 650 : height + 50);
}

static void _renderClear(Color col)
{
    _current_dialog->pixbuf->fill(QColor(col.r * 255.0, col.g * 255.0, col.b * 255.0).rgb());
}

static void _renderDraw(int x, int y, Color col)
{
    _current_dialog->pixbuf->setPixel(x, _current_dialog->pixbuf->height() - 1 - y, QColor(col.r * 255.0, col.g * 255.0, col.b * 255.0).rgb());
}

static void _renderUpdate(double progress)
{
    _current_dialog->area->update();
    _current_dialog->progress_value = progress * 1000.0;
}

class RenderArea:public QWidget
{
public:
    RenderArea(QWidget* parent):
        QWidget(parent)
    {
        setMinimumSize(800, 600);
    }

    void paintEvent(QPaintEvent* event)
    {
        QPainter painter(this);
        painter.drawImage(0, 0, *_current_dialog->pixbuf);
        _current_dialog->progress->setValue(_current_dialog->progress_value);
        _current_dialog->progress->update();
    }
};

DialogRender::DialogRender(QWidget *parent):
    QDialog(parent)
{
    pixbuf = new QImage(1, 1, QImage::Format_ARGB32);
    _current_dialog = this;
    render_thread = NULL;

    setModal(true);
    setWindowTitle("Paysages 3D - Render");
    setLayout(new QVBoxLayout());

    scroll = new QScrollArea(this);
    scroll->setAlignment(Qt::AlignCenter);
    area = new RenderArea(scroll);
    scroll->setWidget(area);
    layout()->addWidget(scroll);

    progress = new QProgressBar(this);
    progress->setMinimum(0);
    progress->setMaximum(1000);
    progress->setValue(0);
    layout()->addWidget(progress);
    progress_value = 0;
}

DialogRender::~DialogRender()
{
    if (render_thread)
    {
        renderInterrupt();
        render_thread->wait();

        renderSetPreviewCallbacks(NULL, NULL, NULL, NULL);

        delete render_thread;
    }
    delete pixbuf;
}

void DialogRender::startRender(int quality, int width, int height)
{
    renderer = sceneryGetStandardRenderer(quality);
    renderSetSize(width, height);
    renderSetPreviewCallbacks(_renderResize, _renderClear, _renderDraw, _renderUpdate);

    render_thread = new RenderThread(&renderer);
    render_thread->start();

    exec();
}

void DialogRender::loadLastRender()
{
    progress->hide();
    renderSetPreviewCallbacks(_renderResize, _renderClear, _renderDraw, _renderUpdate);

    exec();
}
