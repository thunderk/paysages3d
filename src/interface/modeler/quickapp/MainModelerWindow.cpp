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
#include "DiffManager.h"

#include <QQmlEngine>
#include <QGuiApplication>

MainModelerWindow::MainModelerWindow()
{
    scenery = new Scenery();
    scenery->autoPreset();

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

QString MainModelerWindow::getState() const
{
    return rootObject()->property("state").toString();
}

void MainModelerWindow::setState(const QString &stateName)
{
    rootObject()->setProperty("state", stateName);
}

void MainModelerWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (getState() == "Render Dialog")
    {
        if (event->key() == Qt::Key_Escape)
        {
            render_process->stopRender();

            setState("Init");
        }
    }
    else
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
        else if (event->key() == Qt::Key_F12)
        {
            Logs::warning() << "Current scenery dump:" << std::endl << scenery->toString() << std::endl;
        }
        else if (event->key() == Qt::Key_Q)
        {
            if (event->modifiers() & Qt::ControlModifier)
            {
                QGuiApplication::instance()->exit();
            }
        }
        else if (event->key() == Qt::Key_N)
        {
            if (event->modifiers() & Qt::ControlModifier)
            {
                getScenery()->autoPreset();
                renderer->reset();
            }
        }
        else if (event->key() == Qt::Key_S)
        {
            if (event->modifiers() & Qt::ControlModifier)
            {
                getScenery()->saveGlobal("saved.p3d");
            }
        }
        else if (event->key() == Qt::Key_L or event->key() == Qt::Key_O)
        {
            if (event->modifiers() & Qt::ControlModifier)
            {
                getScenery()->loadGlobal("saved.p3d");
                renderer->reset();
            }
        }
        else if (event->key() == Qt::Key_Z)
        {
            if (event->modifiers() & Qt::ControlModifier)
            {
                if (event->modifiers() & Qt::ShiftModifier)
                {
                    getScenery()->getDiffManager()->redo();
                }
                else
                {
                    getScenery()->getDiffManager()->undo();
                }
            }
        }
        else if (event->key() == Qt::Key_Y)
        {
            if (event->modifiers() & Qt::ControlModifier)
            {
                getScenery()->getDiffManager()->undo();
            }
        }
    }
}
