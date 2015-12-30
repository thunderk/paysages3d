#ifndef ATMOSPHEREMODELER_H
#define ATMOSPHEREMODELER_H

#include "modeler_global.h"

#include "BaseModelerTool.h"

namespace paysages {
namespace modeler {

class AtmosphereModeler : protected BaseModelerTool {
  public:
    AtmosphereModeler(MainModelerWindow *window);

    virtual void nodeChanged(const DefinitionNode *node, const DefinitionDiff *diff) override;
};
}
}

#endif // ATMOSPHEREMODELER_H
