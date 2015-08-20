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
    if (not acceptInputs())
    {
        return;
    }

    double factor = getSpeedFactor(event);
    window->getCamera()->processZoom(0.01 * factor * (double)event->angleDelta().y());
}

void OpenGLView::mousePressEvent(QMouseEvent *event)
{
    if (not acceptInputs())
    {
        return;
    }

    mouse_button = event->button();
    mouse_pos = event->windowPos();
}

void OpenGLView::mouseReleaseEvent(QMouseEvent *)
{
    if (not acceptInputs())
    {
        return;
    }

    mouse_button = Qt::NoButton;
}

void OpenGLView::mouseMoveEvent(QMouseEvent *event)
{
    if (not acceptInputs())
    {
        return;
    }

    double factor = getSpeedFactor(event);
    QPointF diff = event->windowPos() - mouse_pos;
    if (mouse_button == Qt::LeftButton)
    {
        window->getCamera()->processPanning(0.002 * factor * diff.x(), 0.001 * factor * diff.y());
    }
    else if (mouse_button == Qt::RightButton)
    {
        window->getCamera()->processScroll(-0.02 * factor * diff.x(), 0.02 * factor * diff.y());
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

bool OpenGLView::acceptInputs() const
{
    return window->getState() != "Render Dialog";
}

double OpenGLView::getSpeedFactor(QInputEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier)
    {
        return 0.2;
    }
    else if (event->modifiers() & Qt::ShiftModifier)
    {
        return 3.0;
    }
    else
    {
        return 1.0;
    }
}
