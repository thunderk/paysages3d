#include "FloatPropertyBind.h"

#include "FloatNode.h"
#include "Logs.h"
#include "MainModelerWindow.h"

#include <cmath>

FloatPropertyBind::FloatPropertyBind(MainModelerWindow *window, const string &object_name, const string &property_name,
                                     FloatNode *node)
    : QObject(window), DefinitionWatcher("FloatPropertyBind " + object_name + "." + property_name), node(node),
      property(property_name) {
    item = window->findQmlObject(object_name);
    if (item) {
        startWatching(node);
        string signal_name("2" + property_name + "Changed()");
        connect(item, signal_name.c_str(), this, SLOT(propertyChanged()));
    } else {
        item = NULL;
        Logs::error("UI") << "Can't find object :" << object_name << endl;
    }
}

void FloatPropertyBind::nodeChanged(const DefinitionNode *, const DefinitionDiff *, const DefinitionNode *) {
    if (item) {
        item->setProperty(property.c_str(), node->getValue());
    }
}

void FloatPropertyBind::propertyChanged() {
    bool ok;
    double value = item->property(property.c_str()).toDouble(&ok);
    if (ok and fabs(value - node->getValue()) > 0.00000001) {
        node->setValue(value);
    }
}
