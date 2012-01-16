#include "dialognoise.h"

#include <QVBoxLayout>
#include <QImage>
#include <QColor>
#include <QPainter>
#include <QScrollArea>

#include "../lib_paysages/shared/functions.h"

DialogNoise::DialogNoise(QWidget *parent, NoiseGenerator* value):
    QDialog(parent)
{
    _base = value;
    _current = noiseCreateGenerator();

    noiseCopy(_base, _current);

    setLayout(new QVBoxLayout());
    _form = new FormNoise(this, _current);
    layout()->addWidget(_form);

    setWindowTitle("Paysages 3D - Noise editor");
}

bool DialogNoise::getNoise(QWidget* parent, NoiseGenerator* noise)
{
    int result;

    DialogNoise* dialog = new DialogNoise(parent, noise);
    result = dialog->exec();

    delete dialog;

    return (result != 0) ? true : false;
}

void DialogNoise::closeEvent(QCloseEvent* e)
{
    reject();
}

void DialogNoise::accept()
{
    noiseCopy(_current, _base);
    reject();
}

void DialogNoise::reject()
{
    noiseDeleteGenerator(_current);
}
