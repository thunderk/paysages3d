#include "mainterrainform.h"
#include "ui_mainterrainform.h"

MainTerrainForm::MainTerrainForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainTerrainForm)
{
    ui->setupUi(this);
}

MainTerrainForm::~MainTerrainForm()
{
    delete ui;
}
