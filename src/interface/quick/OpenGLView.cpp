#include "OpenGLView.h"

#include <QQuickWindow>

// TEMP
#include "OpenGLRenderer.h"
#include "Scenery.h"
static OpenGLRenderer renderer;
static Scenery scenery;

OpenGLView::OpenGLView(QQuickItem *parent) :
    QQuickItem(parent)
{
    initialized = false;

    connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(handleWindowChanged(QQuickWindow*)));

    scenery.autoPreset();
    renderer.setScenery(&scenery);
}

void OpenGLView::handleWindowChanged(QQuickWindow *win)
{
    if (win)
    {
        connect(window(), SIGNAL(beforeRendering()), this, SLOT(paint()), Qt::DirectConnection);

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

    renderer.resize(window()->width(), window()->height());
    renderer.paint();
}
