#ifndef ATMOSPHEREMODELER_H
#define ATMOSPHEREMODELER_H

#include "modeler_global.h"

#include "BaseModelerTool.h"

namespace paysages {
namespace modeler {

class AtmosphereModeler : protected BaseModelerTool {
  public:
    AtmosphereModeler(MainModelerWindow *main);
};
}
}

#endif // ATMOSPHEREMODELER_H
