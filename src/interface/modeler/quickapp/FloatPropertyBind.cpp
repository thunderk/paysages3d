#include "FloatPropertyBind.h"

#include "MainModelerWindow.h"
#include "FloatNode.h"
#include "Logs.h"

#include <cmath>

FloatPropertyBind::FloatPropertyBind(MainModelerWindow *window, const QString &object_name,
                                     const QString &property_name, FloatNode *node)
    : QObject(window), node(node), property(property_name) {
    item = window->findQmlObject(object_name);
    if (item) {
        node->addWatcher(this, true);
        connect(item, SIGNAL(changed(double)), this, SLOT(propertyChanged(double)));
    } else {
        item = NULL;
        Logs::error() << "Can't find object :" << object_name.toStdString() << endl;
    }
}

void FloatPropertyBind::nodeChanged(const DefinitionNode *, const DefinitionDiff *) {
    if (item) {
        item->setProperty(property.toLocal8Bit(), node->getValue());
    }
}

void FloatPropertyBind::propertyChanged(double value) {
    if (fabs(value - node->getValue()) > 0.00000001) {
        node->setValue(value);
    }
}
