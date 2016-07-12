#pragma once

#include "basics_global.h"

namespace paysages {
namespace basics {

class BASICSSHARED_EXPORT BoundingBox {
  public:
    BoundingBox();

    void reset();
    void pushPoint(const Vector3 &point);

  public:
    int empty;
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    double zmin;
    double zmax;
};
}
}
