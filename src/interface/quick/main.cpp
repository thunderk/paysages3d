#include <QGuiApplication>
#include <QQuickView>
#include <QtQml>

#include "OpenGLView.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<OpenGLView>("Paysages", 1, 0, "OpenGLView");

    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:///main.qml"));
    view.show();

    return app.exec();
}
