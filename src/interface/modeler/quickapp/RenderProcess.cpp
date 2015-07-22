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
    rendering = false;
    renderer = NULL;
    render_thread = NULL;

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

    rendering = true;

    if (renderer)
    {
        delete renderer;
    }

    renderer = new SoftwareCanvasRenderer();
    renderer->setScenery(scenery);
    renderer->setConfig(config);

    destination->setCanvas(renderer->getCanvas());

    render_thread = new RenderThread(renderer);
    render_thread->start();
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
        QObject *progress = window->findQmlObject("render_progress");
        if (progress)
        {
            progress->setProperty("value", renderer->getProgress());
        }
    }
}
