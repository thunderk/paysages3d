#include "OpenGLView.h"

#include <QQuickWindow>
#include <QHoverEvent>
#include "MainModelerWindow.h"
#include "OpenGLRenderer.h"
#include "ModelerCameras.h"

OpenGLView::OpenGLView(QQuickItem *parent) :
    QQuickItem(parent)
{
    initialized = false;
    window = NULL;
    renderer = NULL;

    setAcceptedMouseButtons(Qt::AllButtons);

    mouse_button = Qt::NoButton;

    connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(handleWindowChanged(QQuickWindow*)));
    startTimer(50);
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
    if (not initialized or not renderer)
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

void OpenGLView::wheelEvent(QWheelEvent *event)
{
    window->getCamera()->processZoom(0.1 * (double)event->angleDelta().y());
}

void OpenGLView::mousePressEvent(QMouseEvent *event)
{
    mouse_button = event->button();
    mouse_pos = event->windowPos();
}

void OpenGLView::mouseReleaseEvent(QMouseEvent *)
{
    mouse_button = Qt::NoButton;
}

void OpenGLView::mouseMoveEvent(QMouseEvent *event)
{
    QPointF diff = event->windowPos() - mouse_pos;
    if (mouse_button == Qt::MidButton)
    {
        window->getCamera()->processScroll(-0.1 * diff.x(), 0.1 * diff.y());
    }
    mouse_pos = event->windowPos();
}

void OpenGLView::timerEvent(QTimerEvent *)
{
    if (window)
    {
        window->update();
    }
}
