#ifndef BASEMODELERTOOL_H
#define BASEMODELERTOOL_H

#include "modeler_global.h"

#include <memory>

namespace paysages {
namespace modeler {

class BaseModelerTool {
  public:
    BaseModelerTool(MainModelerWindow *main);
    virtual ~BaseModelerTool();

    /**
     * Add an automated two-way binding between a QML int property and a scenery IntNode.
     */
    void addIntBinding(const string &object, const string &property, const string &path);

    /**
     * Add an automated two-way binding between a QML int property and a scenery IntNode.
     */
    void addFloatBinding(const string &object, const string &property, const string &path);

  private:
    class pimpl;
    unique_ptr<pimpl> impl;

    MainModelerWindow *main;
};
}
}

#endif // BASEMODELERTOOL_H
