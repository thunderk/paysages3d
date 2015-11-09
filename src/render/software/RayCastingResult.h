#ifndef RAYCASTINGRESULT_H
#define RAYCASTINGRESULT_H

#include "software_global.h"

#include "Color.h"
#include "Vector3.h"

namespace paysages {
namespace software {

class SOFTWARESHARED_EXPORT RayCastingResult {
  public:
    RayCastingResult();

    bool hit;
    Color hit_color;
    Vector3 hit_location;
};
}
}

#endif // RAYCASTINGRESULT_H
