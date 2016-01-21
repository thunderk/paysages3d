#ifndef BASEMODELERTOOL_H
#define BASEMODELERTOOL_H

#include "modeler_global.h"

#include "DefinitionWatcher.h"

#include <memory>

namespace paysages {
namespace modeler {

class BaseModelerTool : protected DefinitionWatcher {
  public:
    BaseModelerTool(MainModelerWindow *ui);
    virtual ~BaseModelerTool();

    /**
     * Call before destructor, to allow for proper release of resources.
     */
    virtual void destroy();

    /**
     * Add an automated two-way binding between a QML int property and a scenery IntNode.
     *
     * If 'monitor' is true, this tool will also receive the node changes, via the DefinitionWatcher mechanism.
     */
    void addIntBinding(const string &object, const string &property, const string &path, bool monitor = false);

    /**
     * Add an automated two-way binding between a QML int property and a scenery IntNode.
     *
     * If 'monitor' is true, this tool will also receive the node changes, via the DefinitionWatcher mechanism.
     */
    void addFloatBinding(const string &object, const string &property, const string &path, bool monitor = false);

  protected:
    inline MainModelerWindow *getWindow() const {
        return ui;
    }

  private:
    class pimpl;
    unique_ptr<pimpl> impl;

    MainModelerWindow *ui;
};
}
}

#endif // BASEMODELERTOOL_H
