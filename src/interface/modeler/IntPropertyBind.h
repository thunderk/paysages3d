#pragma once

#include "modeler_global.h"

#include "DefinitionWatcher.h"
#include <QObject>

namespace paysages {
namespace modeler {

/**
 * Bind an int Qml property to an IntNode.
 *
 * The IntNode must exist through this object lifetime.
 */
class IntPropertyBind : public QObject, public DefinitionWatcher {
    Q_OBJECT
  public:
    IntPropertyBind(MainModelerWindow *window, const string &object_name, const string &property_name, IntNode *node);

    virtual void nodeChanged(const DefinitionNode *node, const DefinitionDiff *diff,
                             const DefinitionNode *parent) override;

  private slots:
    void propertyChanged();

  private:
    IntNode *node;
    QObject *item;
    string property;
};
}
}
