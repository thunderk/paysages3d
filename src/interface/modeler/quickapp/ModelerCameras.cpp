#include "ModelerCameras.h"

#include "MainModelerWindow.h"
#include "OpenGLRenderer.h"
#include "Scenery.h"
#include "CameraDefinition.h"

ModelerCameras::ModelerCameras(MainModelerWindow *parent):
    QObject(parent), parent(parent)
{
    render = new CameraDefinition();
    topdown = new CameraDefinition();
    current = new CameraDefinition();
    active = render;

    topdown->strafeForward(-10.0);
    topdown->strafeUp(25.0);
    topdown->rotatePitch(-0.8);

    // Watch GUI choice
    QObject *widget = parent->findQmlObject("camera_choice");
    connect(widget, SIGNAL(stateChanged(QString)), this, SLOT(changeActiveCamera(QString)));

    // Start update timer
    startTimer(50);
}

ModelerCameras::~ModelerCameras()
{
    delete current;
    delete render;
    delete topdown;
}

void ModelerCameras::timerEvent(QTimerEvent *)
{
    OpenGLRenderer *renderer = parent->getRenderer();

    current->transitionToAnother(active, 0.1);
    renderer->setCamera(current);
}

void ModelerCameras::changeActiveCamera(const QString &name)
{
    if (name == "Render camera")
    {
        active = render;
    }
    else if (name == "Top-down camera")
    {
        active = topdown;
    }
}
