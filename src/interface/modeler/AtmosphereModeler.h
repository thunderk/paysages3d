#pragma once

#include "modeler_global.h"

#include "BaseModelerTool.h"

namespace paysages {
namespace modeler {

class AtmosphereModeler : public BaseModelerTool {
  public:
    AtmosphereModeler(MainModelerWindow *ui);

    virtual void nodeChanged(const DefinitionNode *node, const DefinitionDiff *diff,
                             const DefinitionNode *parent) override;
};
}
}
