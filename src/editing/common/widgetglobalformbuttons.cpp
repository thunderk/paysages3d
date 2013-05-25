#include "widgetglobalformbuttons.h"
#include "ui_widgetglobalformbuttons.h"

WidgetGlobalFormButtons::WidgetGlobalFormButtons(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetGlobalFormButtons)
{
    ui->setupUi(this);

    connect(ui->button_ok, SIGNAL(clicked()), this, SIGNAL(okClicked()));
    connect(ui->button_cancel, SIGNAL(clicked()), this, SIGNAL(cancelClicked()));
    connect(ui->button_revert, SIGNAL(clicked()), this, SIGNAL(revertClicked()));
}

WidgetGlobalFormButtons::~WidgetGlobalFormButtons()
{
    delete ui;
}
