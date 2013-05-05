#include "widgetglobalformbuttons.h"
#include "ui_widgetglobalformbuttons.h"

WidgetGlobalFormButtons::WidgetGlobalFormButtons(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetGlobalFormButtons)
{
    QPushButton* button;

    ui->setupUi(this);

    button = findChild<QPushButton*>("button_ok");
    if (button)
    {
        connect(button, SIGNAL(clicked()), this, SIGNAL(okClicked()));
    }
    button = findChild<QPushButton*>("button_cancel");
    if (button)
    {
        connect(button, SIGNAL(clicked()), this, SIGNAL(cancelClicked()));
    }
    button = findChild<QPushButton*>("button_revert");
    if (button)
    {
        connect(button, SIGNAL(clicked()), this, SIGNAL(revertClicked()));
    }
}

WidgetGlobalFormButtons::~WidgetGlobalFormButtons()
{
    delete ui;
}
