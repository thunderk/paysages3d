#include <QApplication>
#include "mainwindow.h"

#include "../lib_paysages/shared/functions.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    MainWindow window;

    paysagesInit();

    window.show();

    return app.exec();
}
