#include "RenderProcess.h"

#include "Canvas.h"
#include "CanvasPreview.h"
#include "MainModelerWindow.h"
#include "OpenGLRenderer.h"
#include "RenderConfig.h"
#include "RenderPreviewProvider.h"
#include "RenderProgress.h"
#include "SoftwareCanvasRenderer.h"
#include "Thread.h"
#include <QQuickItem>
#include <QSize>
#include <QTime>

class RenderThread : public Thread {
  public:
    RenderThread(SoftwareCanvasRenderer *renderer) : renderer(renderer) {
    }
    virtual void run() override {
        renderer->render();
    }

  private:
    SoftwareCanvasRenderer *renderer;
};

RenderProcess::RenderProcess(MainModelerWindow *window, RenderPreviewProvider *destination)
    : window(window), destination(destination) {
    has_render = false;
    rendering = false;
    renderer = NULL;
    render_thread = NULL;

    QObject *button_quick = window->findQmlObject("tool_render_quick");
    if (button_quick) {
        connect(button_quick, SIGNAL(clicked()), this, SLOT(startQuickRender()));
    }

    QObject *button_medium = window->findQmlObject("tool_render_medium");
    if (button_medium) {
        connect(button_medium, SIGNAL(clicked()), this, SLOT(startMediumRender()));
    }

    QObject *button_final = window->findQmlObject("tool_render_final");
    if (button_final) {
        connect(button_final, SIGNAL(clicked()), this, SLOT(startFinalRender()));
    }

    QObject *button_show = window->findQmlObject("tool_render_show");
    if (button_show) {
        button_show->setProperty("enabled", false);
        connect(button_show, SIGNAL(clicked()), this, SLOT(showPreviousRender()));
    }

    QObject *button_cancel = window->findQmlObject("render_cancel");
    if (button_cancel) {
        connect(button_cancel, SIGNAL(clicked()), this, SLOT(stopRender()));
    }

    QObject *dialog_save = window->findQmlObject("render_save_dialog");
    if (dialog_save) {
        connect(dialog_save, SIGNAL(saveRequired(QString)), this, SLOT(savePicture(QString)));
    }

    startTimer(100);
}

RenderProcess::~RenderProcess() {
    if (rendering) {
        renderer->interrupt();
    }

    rendering = false;

    if (render_thread) {
        render_thread->join();
        delete render_thread;
    }

    if (renderer) {
        delete renderer;
    }
}

void RenderProcess::startRender(Scenery *scenery, const RenderConfig &config) {
    if (rendering) {
        return;
    }

    // Enable "show last render" button
    window->setQmlProperty("tool_render_show", "enabled", true);
    window->setQmlProperty("render_dialog", "rendering", true);

    has_render = true;
    rendering = true;

    if (renderer) {
        delete renderer;
    }

    renderer = new SoftwareCanvasRenderer(scenery);
    renderer->setConfig(config);

    destination->setCanvas(renderer->getCanvas());

    // Resize preview
    QSize preview_size = getPreviewSize();
    QObject *image = window->findQmlObject("render_preview_image");
    if (image) {
        image->setProperty("width", preview_size.width());
        image->setProperty("height", preview_size.height());
    }

    // Pause OpenGL renderer
    window->getRenderer()->pause();

    // Start render thread
    render_thread = new RenderThread(renderer);
    render_thread->start();

    // Show render dialog
    window->setState("Render Dialog");
}

void RenderProcess::startQuickRender() {
    startRender(window->getScenery(), RenderConfig(480, 270, 1, 3));
}

void RenderProcess::startMediumRender() {
    startRender(window->getScenery(), RenderConfig(800, 450, 1, 7));
}

void RenderProcess::startFinalRender() {
    startRender(window->getScenery(), RenderConfig(1920, 1080, 4, 10));
}

void RenderProcess::showPreviousRender() {
    if (not rendering and has_render) {
        destination->setCanvas(renderer->getCanvas());
        destination->releaseCanvas();

        window->setState("Render Dialog");
    }
}

void RenderProcess::stopRender() {
    if (rendering) {
        renderer->interrupt();
    } else {
        destination->hide();
        window->setState(window->rootObject()->property("previous_state").toString().toStdString());
    }
}

void RenderProcess::savePicture(QString path) {
    renderer->saveToDisk(path.replace("file://", "").toStdString());
}

const QSize RenderProcess::getPreviewSize() {
    if (renderer) {
        return QSize(renderer->getCanvas()->getPreview()->getWidth(), renderer->getCanvas()->getPreview()->getHeight());
    } else {
        return QSize(10, 10);
    }
}

void RenderProcess::timerEvent(QTimerEvent *) {
    if (rendering and renderer->isFinished()) {
        destination->releaseCanvas();
        rendering = false;

        render_thread->join();
        delete render_thread;
        render_thread = NULL;

        window->setQmlProperty("render_dialog", "rendering", false);
        window->getRenderer()->resume();
    }

    if (renderer) {
        QTime t = QTime(0, 0, 0).addMSecs(renderer->getProgressHelper()->getDuration());
        QString info = QString("Elapsed time: ") + t.toString("hh:mm:ss");
        if (rendering) {
            unsigned long remaining = renderer->getProgressHelper()->estimateRemainingTime();
            if (remaining > 0) {
                t = QTime(0, 0, 0).addMSecs(999 + remaining);
                info += " - Remaining: ~" + t.toString("hh:mm:ss");
            }
        }
        window->setQmlProperty("render_timing", "text", info);

        window->setQmlProperty("render_progress", "value", renderer->getProgress());
    }
}
