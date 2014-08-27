#include "OpenGLView.h"

#include <QQuickWindow>
#include <QHoverEvent>

// TEMP
#include "OpenGLRenderer.h"
#include "Scenery.h"
#include "CameraDefinition.h"
static OpenGLRenderer renderer;
static Scenery scenery;

OpenGLView::OpenGLView(QQuickItem *parent) :
    QQuickItem(parent)
{
    initialized = false;
    window = NULL;

    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    setKeepMouseGrab(true);

    scenery.autoPreset();
    renderer.setScenery(&scenery);

    connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(handleWindowChanged(QQuickWindow*)));
    startTimer(250);
}

void OpenGLView::handleWindowChanged(QQuickWindow *win)
{
    if (win)
    {
        window = win;

        connect(window, SIGNAL(beforeRendering()), this, SLOT(paint()), Qt::DirectConnection);

        win->setClearBeforeRendering(false);

        initialized = false;
    }
}

void OpenGLView::paint()
{
    if (not initialized)
    {
        renderer.initialize();
        initialized = true;
    }

    renderer.resize(width(), height());
    renderer.prepareOpenGLState();
    renderer.paint();

    if (window)
    {
        window->resetOpenGLState();
    }
}

void OpenGLView::hoverMoveEvent(QHoverEvent *event)
{
    CameraDefinition camera;
    renderer.getScenery()->getCamera(&camera);

    QPointF diff = event->posF() - event->oldPosF();
    camera.strafeRight(diff.x() * 0.1);
    camera.strafeUp(diff.y() * 0.1);
    camera.validate();

    camera.copy(renderer.render_camera);
    renderer.getScenery()->setCamera(&camera);
    renderer.getScenery()->getCamera(&camera);
    renderer.cameraChangeEvent(&camera);
    if (window)
    {
        window->update();
    }
}

void OpenGLView::timerEvent(QTimerEvent *)
{
    if (window)
    {
        window->update();
    }
}
