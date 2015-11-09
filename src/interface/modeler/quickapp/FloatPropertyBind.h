#ifndef FLOATPROPERTYBIND_H
#define FLOATPROPERTYBIND_H

#include "modeler_global.h"

#include <QObject>
#include "DefinitionWatcher.h"

namespace paysages {
namespace modeler {

/**
 * Bind a float Qml property to a FloatNode.
 *
 * The FloatNode must exist through this object lifetime.
 */
class FloatPropertyBind : public QObject, public DefinitionWatcher {
    Q_OBJECT
  public:
    FloatPropertyBind(MainModelerWindow *window, const QString &object_name, const QString &property_name,
                      FloatNode *node);

    virtual void nodeChanged(const DefinitionNode *node, const DefinitionDiff *diff) override;

  private slots:
    void propertyChanged(double value);

  private:
    FloatNode *node;
    QString property;
    QObject *item;
};
}
}

#endif // FLOATPROPERTYBIND_H
