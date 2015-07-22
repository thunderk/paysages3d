#include "MainModelerWindow.h"

#include "OpenGLView.h"
#include "Scenery.h"
#include "OpenGLRenderer.h"
#include "AtmosphereModeler.h"
#include "WaterModeler.h"
#include "ModelerCameras.h"
#include "RenderPreviewProvider.h"
#include "RenderProcess.h"
#include "RenderConfig.h"

#include <QQmlEngine>
#include <QGuiApplication>

MainModelerWindow::MainModelerWindow()
{
    scenery = new Scenery();
    scenery->autoPreset();
    renderer = new OpenGLRenderer(scenery);

    render_preview_provider = new RenderPreviewProvider();
    render_process = new RenderProcess(render_preview_provider);

    qmlRegisterType<OpenGLView>("Paysages", 1, 0, "OpenGLView");
    engine()->addImageProvider("renderpreviewprovider", render_preview_provider);

    setMinimumSize(QSize(1000, 800));
    setTitle(QObject::tr("Paysages 3D"));
    setResizeMode(QQuickView::SizeRootObjectToView);
    setSource(QUrl("qrc:///main.qml"));

    cameras = new ModelerCameras(this);
    atmosphere = new AtmosphereModeler(this);
    water = new WaterModeler(this);
}

MainModelerWindow::~MainModelerWindow()
{
    delete atmosphere;
    delete water;
    delete cameras;

    delete render_preview_provider;
    delete render_process;

    delete renderer;
    delete scenery;
}

QObject *MainModelerWindow::findQmlObject(const QString &objectName)
{
    return rootObject()->findChild<QObject *>(objectName);
}

void MainModelerWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F5)
    {
        // Start render in a thread
        render_process->startRender(scenery, RenderConfig(400, 300, 1, 3));

        // Resize preview
        QSize preview_size = render_process->getPreviewSize();
        findQmlObject("preview_image")->setProperty("width", preview_size.width());
        findQmlObject("preview_image")->setProperty("height", preview_size.height());

        // Show render dialog
        rootObject()->setProperty("state", QString("Render Dialog"));
    }
    else if (event->key() == Qt::Key_Escape)
    {
        render_process->stopRender();

        rootObject()->setProperty("state", QString("Init"));
    }
    else if (event->key() == Qt::Key_Q)
    {
        if (event->modifiers() & Qt::ControlModifier)
        {
            QGuiApplication::instance()->exit();
        }
    }
}
