#ifndef DEFINITIONDIFF_H
#define DEFINITIONDIFF_H

#include "definition_global.h"

namespace paysages {
namespace definition {

/**
 * Base class for diffs produced by the definition tree.
 *
 * Diffs are used to undo/redo changes.
 */
class DEFINITIONSHARED_EXPORT DefinitionDiff {
  public:
    DefinitionDiff(const DefinitionNode *node);
    DefinitionDiff(const DefinitionDiff *other);
    virtual ~DefinitionDiff();

    inline const string &getTypeName() const {
        return type_name;
    }
    inline const string &getPath() const {
        return path;
    }

    /**
     * Abstract method to build a reversed diff (used for undoing).
     *
     * This diff is allocated and should be freed by the caller.
     */
    virtual DefinitionDiff *newReversed() const = 0;

  private:
    string type_name;
    string path;
};
}
}

#endif // DEFINITIONDIFF_H
