#include "IntPropertyBind.h"

#include "MainModelerWindow.h"
#include "IntNode.h"
#include "Logs.h"

IntPropertyBind::IntPropertyBind(MainModelerWindow *window, const QString &object_name, const QString &property_name, IntNode *node):
    QObject(window), node(node), property(property_name)
{
    item = window->findQmlObject(object_name);
    if (item)
    {
        node->addWatcher(this, true);
        connect(item, SIGNAL(changed(int)), this, SLOT(propertyChanged(int)));
    }
    else
    {
        item = NULL;
        Logs::error() << "Can't find object :" << object_name.toStdString() << std::endl;
    }
}

void IntPropertyBind::nodeChanged(const DefinitionNode *, const DefinitionDiff *)
{
    if (item)
    {
        item->setProperty(property.toLocal8Bit(), node->getValue());
    }
}

void IntPropertyBind::propertyChanged(int value)
{
    if (value != node->getValue())
    {
        node->setValue(value);
    }
}
