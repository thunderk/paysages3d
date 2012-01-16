#include "dialogrender.h"

#include <QVBoxLayout>
#include <QImage>
#include <QColor>
#include <QPainter>
#include <QScrollArea>

#include "../lib_paysages/shared/functions.h"

class RenderThread:public QThread
{
public:
    void run()
    {
        autoRenderSceneTwoPass(0);
    }
};

static DialogRender* _current_dialog;

static void _renderResize(int width, int height)
{
    delete _current_dialog->pixbuf;
    _current_dialog->pixbuf = new QImage(width, height, QImage::Format_ARGB32);
    _current_dialog->area->setMinimumSize(width, height);
    _current_dialog->area->setMaximumSize(width, height);
    _current_dialog->area->resize(width, height);
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

DialogRender::DialogRender(QWidget *parent, int quality, int width, int height):
    QDialog(parent)
{
    QScrollArea* scroll;

    pixbuf = new QImage(1, 1, QImage::Format_ARGB32);
    _current_dialog = this;

    setModal(true);
    setWindowTitle("Paysages 3D - Render");
    setLayout(new QVBoxLayout());

    scroll = new QScrollArea(this);
    scroll->setMinimumSize(width > 800 ? 850 : width + 50, height > 600 ? 650 : height + 50);
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

    renderSetSize(width, height);
    autoSetRenderQuality(quality);

    renderSetPreviewCallbacks(_renderResize, _renderClear, _renderDraw, _renderUpdate);

    render_thread = new RenderThread();
    render_thread->start();
}

void DialogRender::closeEvent(QCloseEvent* e)
{
    renderInterrupt();
    render_thread->wait();

    renderSetPreviewCallbacks(NULL, NULL, NULL, NULL);

    delete render_thread;
    delete pixbuf;
}

void DialogRender::reject()
{
    renderInterrupt();
    render_thread->wait();

    renderSetPreviewCallbacks(NULL, NULL, NULL, NULL);

    delete render_thread;
    delete pixbuf;

    QDialog::reject();
}
