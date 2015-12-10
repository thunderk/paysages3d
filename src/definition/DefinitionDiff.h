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
    virtual ~DefinitionDiff();

    inline const string &getTypeName() const {
        return type_name;
    }
    inline const string &getPath() const {
        return path;
    }

  private:
    string type_name;
    string path;
};
}
}

#endif // DEFINITIONDIFF_H
