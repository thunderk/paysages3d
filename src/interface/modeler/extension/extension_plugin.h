#ifndef EXTENSION_PLUGIN_H
#define EXTENSION_PLUGIN_H

#include <QQmlExtensionPlugin>

class ExtensionPlugin : public QQmlExtensionPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

  public:
    void registerTypes(const char *uri);
};

#endif // EXTENSION_PLUGIN_H
