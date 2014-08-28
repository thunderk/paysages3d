#include "OpenGLView.h"

#include <QQuickWindow>
#include <QHoverEvent>
#include "MainModelerWindow.h"
#include "CameraDefinition.h"
#include "OpenGLRenderer.h"
#include "Scenery.h"

OpenGLView::OpenGLView(QQuickItem *parent) :
    QQuickItem(parent)
{
    initialized = false;
    window = NULL;
    renderer = NULL;

    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    setKeepMouseGrab(true);

    connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(handleWindowChanged(QQuickWindow*)));
    startTimer(250);
}

void OpenGLView::handleWindowChanged(QQuickWindow *win)
{
    if (win)
    {
        window = qobject_cast<MainModelerWindow *>(win);
        if (window)
        {
            renderer = window->getRenderer();

            connect(win, SIGNAL(beforeRendering()), this, SLOT(paint()), Qt::DirectConnection);

            win->setClearBeforeRendering(false);

            initialized = false;
        }
    }
}

void OpenGLView::paint()
{
    if (not initialized)
    {
        renderer->initialize();
        initialized = true;
    }

    renderer->resize(width(), height());
    renderer->prepareOpenGLState();
    renderer->paint();

    if (window)
    {
        window->resetOpenGLState();
    }
}

void OpenGLView::hoverMoveEvent(QHoverEvent *event)
{
    if (!renderer)
    {
        return;
    }

    CameraDefinition camera;
    renderer->getScenery()->getCamera(&camera);

    QPointF diff = event->posF() - event->oldPosF();
    camera.strafeRight(diff.x() * 0.1);
    camera.strafeUp(diff.y() * 0.1);
    camera.validate();

    camera.copy(renderer->render_camera);
    renderer->getScenery()->setCamera(&camera);
    renderer->getScenery()->getCamera(&camera);
    renderer->cameraChangeEvent(&camera);
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
