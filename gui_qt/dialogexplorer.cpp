#include "dialogexplorer.h"

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

DialogExplorer::DialogExplorer(QWidget* parent, CameraDefinition* camera, bool camera_validable) : QDialog(parent)
{
    QWidget* panel;
    QPushButton* button;

    setModal(true);
    setWindowTitle(tr("Paysages 3D - Explore"));
    setLayout(new QHBoxLayout());

    _wanderer = new WidgetExplorer(this, camera);
    layout()->addWidget(_wanderer);

    panel = new QWidget(this);
    panel->setLayout(new QVBoxLayout());
    panel->setMaximumWidth(230);
    
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

