#include "dialogrender.h"

#include <QVBoxLayout>
#include <QImage>
#include <QColor>
#include <QPainter>
#include "tools.h"

#include "../lib_paysages/scenery.h"
#include "../lib_paysages/auto.h"

static DialogRender* _current_dialog;

static void _renderStart(int width, int height, Color background)
{
    delete _current_dialog->pixbuf;
    _current_dialog->pixbuf = new QImage(width, height, QImage::Format_ARGB32);
    _current_dialog->pixbuf->fill(colorToQColor(background).rgb());
}

static void _renderDraw(int x, int y, Color col)
{
    _current_dialog->pixbuf->setPixel(x, _current_dialog->pixbuf->height() - 1 - y, colorToQColor(col).rgb());
}

static void _renderUpdate(double progress)
{
    _current_dialog->area->update();
    _current_dialog->progress_value = progress * 1000.0;
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

    void paintEvent(QPaintEvent* event)
    {
        QPainter painter(this);
        painter.drawImage(0, 0, *_current_dialog->pixbuf);
        _current_dialog->progress->setValue(_current_dialog->progress_value);
        _current_dialog->progress->update();
    }
};

DialogRender::DialogRender(QWidget *parent, Renderer* renderer):
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint)
{
    pixbuf = new QImage(1, 1, QImage::Format_ARGB32);
    _current_dialog = this;
    render_thread = NULL;
    _renderer = renderer;

    setModal(true);
    setWindowTitle(tr("Paysages 3D - Render"));
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
    
    // TEMP
    progress->hide();
}

DialogRender::~DialogRender()
{
    if (render_thread)
    {
        rendererInterrupt(_renderer);
        render_thread->wait();

        delete render_thread;
    }
    delete pixbuf;
}

void DialogRender::startRender(RenderParams params)
{
    applyRenderSize(params.width, params.height);
    rendererSetPreviewCallbacks(_renderer, _renderStart, _renderDraw, _renderUpdate);

    render_thread = new RenderThread(_renderer, params);
    render_thread->start();

    exec();
}

void DialogRender::loadLastRender()
{
    applyRenderSize(_renderer->render_width, _renderer->render_height);
    progress->hide();
    rendererSetPreviewCallbacks(_renderer, _renderStart, _renderDraw, _renderUpdate);

    exec();
}

void DialogRender::applyRenderSize(int width, int height)
{
    area->setMinimumSize(width, height);
    area->setMaximumSize(width, height);
    area->resize(width, height);
    scroll->setMinimumSize(width > 800 ? 820 : width + 20, height > 600 ? 620 : height + 20);
}
