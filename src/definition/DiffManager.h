#ifndef DIFFMANAGER_H
#define DIFFMANAGER_H

#include "definition_global.h"

#include <map>
#include <vector>

namespace paysages {
namespace definition {

/**
 * Manager of diffs produced by a definition tree.
 *
 * Watchers can register themselves to received these diffs.
 */
class DEFINITIONSHARED_EXPORT DiffManager
{
public:
    DiffManager(DefinitionNode *tree);

    /**
     * Add a watcher for a specific node.
     *
     * The watcher reference must remain valid through the manager lifetime.
     */
    void addWatcher(const DefinitionNode *node, DefinitionWatcher *watcher);

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

private:
    DefinitionNode *tree;
    int undone;
    std::vector<const DefinitionDiff *> diffs;
    std::map<const DefinitionNode *, std::vector<DefinitionWatcher *>> watchers;
};

}
}

#endif // DIFFMANAGER_H
