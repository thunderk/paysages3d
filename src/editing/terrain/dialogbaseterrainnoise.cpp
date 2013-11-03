#include "dialogbaseterrainnoise.h"
#include "ui_dialogbaseterrainnoise.h"

#include "NoiseGenerator.h"

DialogBaseTerrainNoise::DialogBaseTerrainNoise(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogBaseTerrainNoise)
{
    ui->setupUi(this);

    _original = 0;
    _modified = new NoiseGenerator();
}

DialogBaseTerrainNoise::~DialogBaseTerrainNoise()
{
    delete ui;
    delete _modified;
}

void DialogBaseTerrainNoise::setNoise(NoiseGenerator* noise)
{
    _original = noise;
    noise->copy(_modified);
}

int DialogBaseTerrainNoise::editNoise(QWidget* parent, NoiseGenerator* noise)
{
    DialogBaseTerrainNoise dialog(parent);
    dialog.setNoise(noise);
    return dialog.exec();
}
