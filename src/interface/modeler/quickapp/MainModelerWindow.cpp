#include "MainModelerWindow.h"

#include "Logs.h"
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

    Logs::debug() << "Initialized scenery:\n" << scenery->toString() << std::endl;

    renderer = new OpenGLRenderer(scenery);

    render_preview_provider = new RenderPreviewProvider();

    qmlRegisterType<OpenGLView>("Paysages", 1, 0, "OpenGLView");
    engine()->addImageProvider("renderpreviewprovider", render_preview_provider);

    setMinimumSize(QSize(1000, 800));
    setTitle(QObject::tr("Paysages 3D"));
    setResizeMode(QQuickView::SizeRootObjectToView);
    setSource(QUrl("qrc:///main.qml"));

    cameras = new ModelerCameras(this);
    atmosphere = new AtmosphereModeler(this);
    water = new WaterModeler(this);

    render_process = new RenderProcess(this, render_preview_provider);
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

void MainModelerWindow::setQmlProperty(const QString &objectName, const QString &propertyName, const QVariant &value)
{
    QObject *item = findQmlObject(objectName);
    if (item)
    {
        item->setProperty(propertyName.toLocal8Bit(), value);
    }
}

void MainModelerWindow::setState(const QString &stateName)
{
    rootObject()->setProperty("state", stateName);
}

void MainModelerWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F5)
    {
        // Start render in a thread
        if (event->modifiers() & Qt::ControlModifier)
        {
            render_process->startFinalRender();
        }
        else
        {
            render_process->startQuickRender();
        }
    }
    else if (event->key() == Qt::Key_F6)
    {
        render_process->showPreviousRender();
    }
    else if (event->key() == Qt::Key_Escape)
    {
        render_process->stopRender();

        setState("Init");
    }
    else if (event->key() == Qt::Key_Q)
    {
        if (event->modifiers() & Qt::ControlModifier)
        {
            QGuiApplication::instance()->exit();
        }
    }
}
