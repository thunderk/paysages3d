#include "MainModelerWindow.h"

#include "OpenGLView.h"
#include "Scenery.h"
#include "OpenGLRenderer.h"

MainModelerWindow::MainModelerWindow()
{
    scenery = new Scenery();
    scenery->autoPreset();
    renderer = new OpenGLRenderer(scenery);

    qmlRegisterType<OpenGLView>("Paysages", 1, 0, "OpenGLView");

    setTitle(QObject::tr("Paysages 3D"));
    setResizeMode(QQuickView::SizeRootObjectToView);
    setSource(QUrl("qrc:///main.qml"));
}

MainModelerWindow::~MainModelerWindow()
{
    delete renderer;
    delete scenery;
}
