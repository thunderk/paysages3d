#include "RenderProcess.h"

#include <QSize>
#include "MainModelerWindow.h"
#include "SoftwareCanvasRenderer.h"
#include "RenderPreviewProvider.h"
#include "RenderConfig.h"
#include "Thread.h"
#include "Canvas.h"
#include "CanvasPreview.h"

class RenderThread: public Thread
{
public:
    RenderThread(SoftwareCanvasRenderer *renderer): renderer(renderer)
    {
    }
    virtual void run() override
    {
        renderer->render();
    }
private:
    SoftwareCanvasRenderer *renderer;
};

RenderProcess::RenderProcess(MainModelerWindow *window, RenderPreviewProvider *destination):
    window(window), destination(destination)
{
    has_render = false;
    rendering = false;
    renderer = NULL;
    render_thread = NULL;

    QObject *button_quick = window->findQmlObject("tool_render_quick");
    if (button_quick) {
        connect(button_quick, SIGNAL(clicked()), this, SLOT(startQuickRender()));
    }

    startTimer(100);
}

RenderProcess::~RenderProcess()
{
    if (rendering)
    {
        renderer->interrupt();
    }

    rendering = false;

    if (render_thread)
    {
        render_thread->join();
        delete render_thread;
    }

    if (renderer)
    {
        delete renderer;
    }
}

void RenderProcess::startRender(Scenery *scenery, const RenderConfig &config)
{
    if (rendering)
    {
        return;
    }

    has_render = true;
    rendering = true;

    if (renderer)
    {
        delete renderer;
    }

    renderer = new SoftwareCanvasRenderer();
    renderer->setScenery(scenery);
    renderer->setConfig(config);

    destination->setCanvas(renderer->getCanvas());

    // Resize preview
    QSize preview_size = getPreviewSize();
    QObject *image = window->findQmlObject("preview_image");
    if (image) {
        image->setProperty("width", preview_size.width());
        image->setProperty("height", preview_size.height());
    }

    // Start render thread
    render_thread = new RenderThread(renderer);
    render_thread->start();

    // Show render dialog
    window->setState("Render Dialog");
}

void RenderProcess::startQuickRender()
{
    startRender(window->getScenery(), RenderConfig(400, 300, 1, 3));
}

void RenderProcess::startFinalRender()
{
    startRender(window->getScenery(), RenderConfig(1920, 1080, 4, 8));
}

void RenderProcess::showPreviousRender()
{
    if (not rendering and has_render)
    {
        window->setState("Render Dialog");
    }
}

void RenderProcess::stopRender()
{
    if (rendering)
    {
        renderer->interrupt();
    }
}

const QSize RenderProcess::getPreviewSize()
{
    if (renderer)
    {
        return QSize(renderer->getCanvas()->getPreview()->getWidth(), renderer->getCanvas()->getPreview()->getHeight());
    }
    else
    {
        return QSize(10, 10);
    }
}

void RenderProcess::timerEvent(QTimerEvent *)
{
    if (rendering and renderer->isFinished())
    {
        destination->releaseCanvas();
        rendering = false;

        render_thread->join();
        delete render_thread;
        render_thread = NULL;
    }

    if (renderer)
    {
        window->setQmlProperty("render_progress", "value", renderer->getProgress());
    }
}
