#ifndef DIFFMANAGER_H
#define DIFFMANAGER_H

#include "definition_global.h"

#include <memory>

namespace paysages {
namespace definition {

/**
 * Manager of diffs produced by a definition tree.
 *
 * Watchers can register themselves to received these diffs.
 */
class DEFINITIONSHARED_EXPORT DiffManager {
  public:
    DiffManager(DefinitionNode *tree);
    ~DiffManager();

    /**
     * Get the total number of diff stored.
     */
    unsigned long getDiffCount(int include_undone = true) const;

    /**
     * Add a watcher for a specific node.
     *
     * The watcher reference must remain valid through the manager lifetime.
     */
    void addWatcher(const DefinitionNode *node, DefinitionWatcher *watcher);

    /**
     * Remove a watcher from all nodes.
     */
    void removeWatcher(DefinitionWatcher *watcher);

    /**
     * Get the number of watchers registered for a given node.
     */
    unsigned long getWatcherCount(const DefinitionNode *node);

    /**
     * Add a new diff of a node to the change flow.
     */
    void addDiff(DefinitionNode *node, const DefinitionDiff *diff);

    /**
     * Undo a diff in the definition tree.
     */
    void undo();

    /**
     * Redo a diff in the definition tree, undone by undo().
     */
    void redo();

  protected:
    /**
     * Publish a diff to the registered watchers.
     *
     * The diff will be published to the watcher of the node, and to the watchers of all
     * parents, up to the definition tree root.
     */
    void publishToWatchers(const DefinitionNode *node, const DefinitionDiff *diff);

  private:
    class pimpl;
    unique_ptr<pimpl> impl;
};
}
}

#endif // DIFFMANAGER_H
