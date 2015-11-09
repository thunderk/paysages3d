#include <QGuiApplication>

#include "MainModelerWindow.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    MainModelerWindow view;
    view.show();

    return app.exec();
}
