#ifndef DEFINITIONWATCHER_H
#define DEFINITIONWATCHER_H

#include "definition_global.h"

namespace paysages {
namespace definition {

/**
 * Base class for watchers of the definition tree.
 *
 * Watchers will be registered in DiffManager to receive DefinitionDiff objects.
 */
class DEFINITIONSHARED_EXPORT DefinitionWatcher {
  public:
    DefinitionWatcher();

    /**
     * Abstract method called when a node changed.
     */
    virtual void nodeChanged(const DefinitionNode *node, const DefinitionDiff *diff) = 0;

  protected:
    /**
     * Start watching a path in a definition tree.
     */
    void startWatching(const DefinitionNode *root, const std::string &path, bool init_diff = true);
};
}
}

#endif // DEFINITIONWATCHER_H
