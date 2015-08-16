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
class DEFINITIONSHARED_EXPORT DefinitionDiff
{
public:
    DefinitionDiff(const DefinitionNode *node);

    inline const std::string &getTypeName() const {return type_name;}
    inline const std::string &getPath() const {return path;}

private:
    std::string type_name;
    std::string path;
};

}
}

#endif // DEFINITIONDIFF_H
