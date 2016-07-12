#pragma once

#include "software_global.h"

namespace paysages {
namespace software {

typedef RayCastingResult (*FuncGeneralCastRay)(SoftwareRenderer *renderer, const Vector3 &start,
                                               const Vector3 &direction);

class SOFTWARESHARED_EXPORT RayCastingManager {
  public:
    RayCastingManager();
};
}
}
