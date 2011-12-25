#include "formwater.h"
#include "ui_formwater.h"

FormWater::FormWater(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormWater)
{
    ui->setupUi(this);

    this->findChild<QToolBox*>("water_configs")->setCurrentIndex(0);
}

FormWater::~FormWater()
{
    delete ui;
}
