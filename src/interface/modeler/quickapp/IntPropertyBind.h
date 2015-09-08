#ifndef INTPROPERTYBIND_H
#define INTPROPERTYBIND_H

#include "modeler_global.h"

#include <QObject>
#include "DefinitionWatcher.h"

namespace paysages {
namespace modeler {

/**
 * Bind an int Qml property to an IntNode.
 *
 * The IntNode must exist through this object lifetime.
 */
class IntPropertyBind : public QObject, public DefinitionWatcher
{
    Q_OBJECT
public:
    IntPropertyBind(MainModelerWindow *window, const QString &object_name, const QString &property_name, IntNode *node);

    virtual void nodeChanged(const DefinitionNode *node, const DefinitionDiff *diff) override;

private slots:
    void propertyChanged(int value);

private:
    IntNode *node;
    QString property;
    QObject *item;
};

}
}
#endif // INTPROPERTYBIND_H
