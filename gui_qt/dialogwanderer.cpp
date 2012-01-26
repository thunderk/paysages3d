#include "dialogwanderer.h"

#include <QGLWidget>
#include <QVBoxLayout>

DialogWanderer::DialogWanderer(QWidget *parent):
    QDialog(parent)
{
    setModal(true);
    setWindowTitle("Paysages 3D - Explore");
    setLayout(new QVBoxLayout());
}

DialogWanderer::~DialogWanderer()
{
}
