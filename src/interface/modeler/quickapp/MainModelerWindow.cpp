#include "MainModelerWindow.h"

#include "OpenGLView.h"
#include "Scenery.h"
#include "OpenGLRenderer.h"
#include "AtmosphereModeler.h"
#include "WaterModeler.h"

MainModelerWindow::MainModelerWindow()
{
    scenery = new Scenery();
    scenery->autoPreset();
    renderer = new OpenGLRenderer(scenery);

    qmlRegisterType<OpenGLView>("Paysages", 1, 0, "OpenGLView");

    setTitle(QObject::tr("Paysages 3D"));
    setResizeMode(QQuickView::SizeRootObjectToView);
    setSource(QUrl("qrc:///main.qml"));

    atmosphere = new AtmosphereModeler(this);
    water = new WaterModeler(this);
}

MainModelerWindow::~MainModelerWindow()
{
    delete atmosphere;
    delete water;

    delete renderer;
    delete scenery;
}

QObject *MainModelerWindow::findQmlObject(const QString &objectName)
{
    return rootObject()->findChild<QObject *>(objectName);
}
