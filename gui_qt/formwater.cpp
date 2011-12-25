#include "formwater.h"
#include "ui_formwater.h"

FormWater::FormWater(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormWater)
{
    ui->setupUi(this);
}

FormWater::~FormWater()
{
    delete ui;
}
