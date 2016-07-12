#pragma once

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
    FloatPropertyBind(MainModelerWindow *window, const string &object_name, const string &property_name,
                      FloatNode *node);

    virtual void nodeChanged(const DefinitionNode *node, const DefinitionDiff *diff,
                             const DefinitionNode *parent) override;

  private slots:
    void propertyChanged();

  private:
    FloatNode *node;
    string property;
    QObject *item;
};
}
}
