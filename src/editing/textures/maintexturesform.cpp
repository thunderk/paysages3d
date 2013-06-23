#include "maintexturesform.h"
#include "ui_maintexturesform.h"

MainTexturesForm::MainTexturesForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainTexturesForm)
{
    ui->setupUi(this);
}

MainTexturesForm::~MainTexturesForm()
{
    delete ui;
}
