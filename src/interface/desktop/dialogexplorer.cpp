#include "dialogexplorer.h"

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QSlider>
#include "WidgetExplorer.h"
#include "DesktopScenery.h"
#include "CameraDefinition.h"

DialogExplorer::DialogExplorer(QWidget* parent, CameraDefinition* camera, bool camera_validable, Scenery* scenery) : QDialog(parent)
{
    QWidget* panel;
    QPushButton* button;
    QLabel* label;

    setModal(true);
    setWindowTitle(tr("Paysages 3D - Explore"));
    setLayout(new QHBoxLayout());

    if (!scenery)
    {
        scenery = DesktopScenery::getCurrent();
    }

    _wanderer = new WidgetExplorer(this, camera, scenery);
    layout()->addWidget(_wanderer);

    panel = new QWidget(this);
    panel->setLayout(new QVBoxLayout());
    panel->setMaximumWidth(230);

    label = new QLabel(tr("Field of vision"), panel);
    label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    panel->layout()->addWidget(label);
    QSlider* slider = new QSlider(Qt::Horizontal, panel);
    slider->setRange(0, 1000);
    slider->setValue((int)(1000.0 * (camera->getPerspective().yfov - 0.7) / 1.0));
    slider->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(fovChanged(int)));
    panel->layout()->addWidget(slider);

    panel->layout()->addWidget(new QLabel(tr("COMMANDS\n\nLeft click : Look around\nRight click : Pan (adjust framing)\nWheel : Move forward/backward\nHold SHIFT : Faster\nHold CTRL : Slower"), panel));

    button = new QPushButton(tr("Reset camera"), panel);
    panel->layout()->addWidget(button);
    QObject::connect(button, SIGNAL(clicked()), _wanderer, SLOT(resetCamera()));

    if (camera_validable)
    {
        button = new QPushButton(tr("Validate as render camera"), panel);
        panel->layout()->addWidget(button);
        QObject::connect(button, SIGNAL(clicked()), this, SLOT(validateCamera()));
    }

    button = new QPushButton(tr("Close"), panel);
    panel->layout()->addWidget(button);
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(reject()));

    layout()->addWidget(panel);

    resize(900, 600);
}

DialogExplorer::~DialogExplorer()
{
}

void DialogExplorer::validateCamera()
{
    _wanderer->validateCamera();
    accept();
}

void DialogExplorer::fovChanged(int value)
{
    _wanderer->setCameraFov(0.7 + 1.0 * ((double)value) / 1000.0);
}

