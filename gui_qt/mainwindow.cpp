#include "mainwindow.h"
#include "formwater.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QTabWidget* tabs;

    ui->setupUi(this);

    tabs = this->findChild<QTabWidget*>("tabWidget");
    tabs->addTab(new FormWater(tabs), "Water");
}

MainWindow::~MainWindow()
{
    delete ui;
}
