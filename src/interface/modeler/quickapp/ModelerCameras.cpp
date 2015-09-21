#include "ModelerCameras.h"

#include "MainModelerWindow.h"
#include "OpenGLRenderer.h"
#include "Scenery.h"
#include "CameraDefinition.h"
#include "AtmosphereDefinition.h"
#include "FloatNode.h"
#include "AtmosphereRenderer.h"

ModelerCameras::ModelerCameras(MainModelerWindow *parent):
    QObject(parent), parent(parent)
{
    render = new CameraDefinition();
    topdown = new CameraDefinition();
    current = new CameraDefinition();
    tool = new CameraDefinition();
    active = render;
    tool_mode = TOOL_NONE;

    // Watch GUI choice
    parent->connectQmlSignal("camera_choice", SIGNAL(stateChanged(QString)), this, SLOT(changeActiveCamera(QString)));
    parent->connectQmlSignal("ui", SIGNAL(mainToolChanged(QString)), this, SLOT(toolChanged(QString)));

    // Validate to apply initial camera to scenery
    validate();

    // Start update timer
    startTimer(50);
}

ModelerCameras::~ModelerCameras()
{
    delete current;
    delete render;
    delete topdown;
    delete tool;
}

void ModelerCameras::processZoom(double value)
{
    active->strafeForward(value);

    validate();
}

void ModelerCameras::processScroll(double xvalue, double yvalue)
{
    active->strafeRight(xvalue);
    active->strafeUp(yvalue);

    validate();
}

void ModelerCameras::processPanning(double xvalue, double yvalue)
{
    active->rotateYaw(xvalue);
    active->rotatePitch(yvalue);

    validate();
}

void ModelerCameras::startSunTool()
{
    tool_mode = TOOL_SUN;
    current->copy(tool);
    active = tool;

    parent->getScenery()->getAtmosphere()->propDayTime()->addWatcher(this, true);
}

void ModelerCameras::endTool()
{
    active = render;
    tool_mode = TOOL_NONE;

    validate();
}

void ModelerCameras::timerEvent(QTimerEvent *)
{
    current->transitionToAnother(active, 0.5);
    parent->getScenery()->keepCameraAboveGround(current);
    parent->getRenderer()->setCamera(current);
}

void ModelerCameras::nodeChanged(const DefinitionNode *node, const DefinitionDiff *)
{
    if (node->getPath() == "/atmosphere/daytime" && tool_mode == TOOL_SUN)
    {
        Vector3 direction = parent->getRenderer()->getAtmosphereRenderer()->getSunDirection();
        tool->setTarget(tool->getLocation().add(direction));
    }
}

void ModelerCameras::validate()
{
    parent->getScenery()->keepCameraAboveGround(active);

    parent->getScenery()->keepCameraAboveGround(current);
    parent->getRenderer()->setCamera(current);

    if (active == render)
    {
        parent->getScenery()->setCamera(active);
    }
}

void ModelerCameras::changeActiveCamera(const QString &name)
{
    if (name == "Render camera")
    {
        active = render;
    }
    else if (name == "Top-down camera")
    {
        render->copy(topdown);

        topdown->strafeForward(-10.0);
        topdown->strafeUp(25.0);
        topdown->rotatePitch(-0.8);

        topdown->validate();

        active = topdown;
    }
}

void ModelerCameras::toolChanged(const QString &tool)
{
    if (tool.isEmpty())
    {
        endTool();
    }
    else if (tool == "sun")
    {
        startSunTool();
    }
}
