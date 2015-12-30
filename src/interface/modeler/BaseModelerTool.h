#ifndef BASEMODELERTOOL_H
#define BASEMODELERTOOL_H

#include "modeler_global.h"

#include "DefinitionWatcher.h"

#include <memory>

namespace paysages {
namespace modeler {

class BaseModelerTool : public DefinitionWatcher {
  public:
    BaseModelerTool(MainModelerWindow *window);
    virtual ~BaseModelerTool();

    /**
     * Add an automated two-way binding between a QML int property and a scenery IntNode.
     *
     * If *monitor* is true, this tool will also receive the node changes, via the DefinitionWatcher mechanism.
     */
    void addIntBinding(const string &object, const string &property, const string &path, bool monitor = false);

    /**
     * Add an automated two-way binding between a QML int property and a scenery IntNode.
     *
     * If *monitor* is true, this tool will also receive the node changes, via the DefinitionWatcher mechanism.
     */
    void addFloatBinding(const string &object, const string &property, const string &path, bool monitor = false);

  protected:
    inline MainModelerWindow *getWindow() const {
        return window;
    }

  private:
    class pimpl;
    unique_ptr<pimpl> impl;

    MainModelerWindow *window;
};
}
}

#endif // BASEMODELERTOOL_H
