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
    RenderThread(Renderer* renderer, int width, int height, int quality):QThread()
    {
        _renderer = renderer;
        _width = width;
        _height = height;
        _quality = quality;
    }
    void run()
    {
        rendererStart(_renderer, _width, _height, _quality);
    }
private:
    Renderer* _renderer;
    int _width;
    int _height;
    int _quality;
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

DialogRender::DialogRender(QWidget *parent):
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint)
{
    pixbuf = new QImage(1, 1, QImage::Format_ARGB32);
    _current_dialog = this;
    render_thread = NULL;
    renderer = sceneryCreateStandardRenderer();

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
        rendererInterrupt(&renderer);
        render_thread->wait();

        rendererSetPreviewCallbacks(&renderer, NULL, NULL, NULL);

        delete render_thread;
    }
    rendererDelete(&renderer);
    delete pixbuf;
}

void DialogRender::startRender(int quality, int width, int height)
{
    delete pixbuf;
    pixbuf = new QImage(width, height, QImage::Format_ARGB32);
    area->setMinimumSize(width, height);
    area->setMaximumSize(width, height);
    area->resize(width, height);
    scroll->setMinimumSize(width > 800 ? 850 : width + 50, height > 600 ? 650 : height + 50);
    
    rendererSetPreviewCallbacks(&renderer, _renderStart, _renderDraw, _renderUpdate);

    render_thread = new RenderThread(&renderer, width, height, quality);
    render_thread->start();

    exec();
}

void DialogRender::loadLastRender()
{
    progress->hide();
    //renderSetPreviewCallbacks(_renderStart, _renderDraw, _renderUpdate);

    exec();
}
