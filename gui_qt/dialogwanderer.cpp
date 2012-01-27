#include "dialogwanderer.h"

#include <QVBoxLayout>
#include "widgetwanderer.h"

DialogWanderer::DialogWanderer(QWidget* parent, CameraDefinition* camera):
    QDialog(parent)
{
    setModal(true);
    setWindowTitle("Paysages 3D - Explore");
    setLayout(new QVBoxLayout());

    layout()->addWidget(new WidgetWanderer(this, camera));

    resize(700, 530);
}

DialogWanderer::~DialogWanderer()
{
}

