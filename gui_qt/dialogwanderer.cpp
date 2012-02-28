#include "dialogwanderer.h"

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include "widgetwanderer.h"

DialogWanderer::DialogWanderer(QWidget* parent, CameraDefinition* camera, bool camera_validable) : QDialog(parent)
{
    QWidget* panel;
    QPushButton* button;

    setModal(true);
    setWindowTitle(tr("Paysages 3D - Explore"));
    setLayout(new QHBoxLayout());

    _wanderer = new WidgetWanderer(this, camera);
    layout()->addWidget(_wanderer);

    panel = new QWidget(this);
    panel->setLayout(new QVBoxLayout());
    panel->setMaximumWidth(200);

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

DialogWanderer::~DialogWanderer()
{
}

void DialogWanderer::validateCamera()
{
    _wanderer->validateCamera();
    reject();
}

