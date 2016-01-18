#ifndef ATMOSPHEREMODELER_H
#define ATMOSPHEREMODELER_H

#include "modeler_global.h"

#include "BaseModelerTool.h"

namespace paysages {
namespace modeler {

class AtmosphereModeler : protected BaseModelerTool {
  public:
    AtmosphereModeler(MainModelerWindow *ui);

    virtual void nodeChanged(const DefinitionNode *node, const DefinitionDiff *diff,
                             const DefinitionNode *parent) override;
};
}
}

#endif // ATMOSPHEREMODELER_H
