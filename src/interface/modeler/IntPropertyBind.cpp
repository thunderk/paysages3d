#include "IntPropertyBind.h"

#include "IntNode.h"
#include "Logs.h"
#include "MainModelerWindow.h"

IntPropertyBind::IntPropertyBind(MainModelerWindow *window, const string &object_name, const string &property_name,
                                 IntNode *node)
    : QObject(window), DefinitionWatcher("IntPropertyBind " + object_name + "." + property_name), node(node),
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

void IntPropertyBind::nodeChanged(const DefinitionNode *, const DefinitionDiff *, const DefinitionNode *) {
    if (item) {
        item->setProperty(property.c_str(), node->getValue());
    }
}

void IntPropertyBind::propertyChanged() {
    bool ok;
    int value = item->property(property.c_str()).toInt(&ok);
    if (ok and value != node->getValue()) {
        node->setValue(value);
    }
}
