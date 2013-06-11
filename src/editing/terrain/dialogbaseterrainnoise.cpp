#include "dialogbaseterrainnoise.h"
#include "ui_dialogbaseterrainnoise.h"

DialogBaseTerrainNoise::DialogBaseTerrainNoise(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogBaseTerrainNoise)
{
    ui->setupUi(this);

    _original = 0;
    _modified = noiseCreateGenerator();
}

DialogBaseTerrainNoise::~DialogBaseTerrainNoise()
{
    delete ui;
    noiseDeleteGenerator(_modified);
}

void DialogBaseTerrainNoise::setNoise(NoiseGenerator* noise)
{
    _original = noise;
    noiseCopy(noise, _modified);
}

int DialogBaseTerrainNoise::editNoise(QWidget* parent, NoiseGenerator* noise)
{
    DialogBaseTerrainNoise dialog(parent);
    dialog.setNoise(noise);
    return dialog.exec();
}
