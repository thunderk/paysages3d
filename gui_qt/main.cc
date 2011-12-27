#include <QApplication>
#include "mainwindow.h"
#include "preview.h"

#include "../lib_paysages/shared/functions.h"

int main(int argc, char** argv)
{
    paysagesInit();

    QApplication app(argc, argv);
    MainWindow window;

    window.show();

    Preview::startUpdater();

    return app.exec();
}
