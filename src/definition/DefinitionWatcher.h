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
class DEFINITIONSHARED_EXPORT DefinitionWatcher
{
public:
    DefinitionWatcher();
};

}
}

#endif // DEFINITIONWATCHER_H
