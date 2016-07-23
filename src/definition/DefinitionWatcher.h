#pragma once

#include "definition_global.h"

#include <set>
#include <string>

namespace paysages {
namespace definition {

/**
 * Base class for watchers of the definition tree.
 *
 * Watchers will be registered in DiffManager to receive DefinitionDiff objects.
 */
class DEFINITIONSHARED_EXPORT DefinitionWatcher {
  public:
    DefinitionWatcher(const string &name);
    virtual ~DefinitionWatcher();

    /**
     * Unregister from all watched nodes.
     *
     * This MUST be called before destructor.
     *
     * A watcher should also make sure it is not unregistered as a direct result of a nodeChanged call,
     * or this may cause a deadlock.
     */
    void unregister();

    /**
     * Abstract method called when a node changed.
     *
     * *parent* is the node that is watched (useful if *node* is a sub-node).
     */
    virtual void nodeChanged(const DefinitionNode *node, const DefinitionDiff *diff, const DefinitionNode *parent);

  protected:
    /**
     * Start watching a path in a definition tree.
     */
    void startWatchingPath(const DefinitionNode *root, const string &path, bool init_diff = true);

    /**
     * Start watching a node.
     */
    void startWatching(const DefinitionNode *node, bool init_diff = true);

    /**
     * Abstract convenience to receive integer node changes.
     */
    virtual void intNodeChanged(const string &path, int new_value, int old_value);

    /**
     * Abstract convenience to receive float node changes.
     */
    virtual void floatNodeChanged(const string &path, double new_value, double old_value);

  private:
    string name;
    set<DiffManager *> registered_to;
};
}
}
