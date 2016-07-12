#pragma once

#include "software_global.h"

#include "Vector3.h"
#include "SurfaceMaterial.h"

namespace paysages {
namespace software {

/**
 * Result of a vegetation lookup.
 */
class SOFTWARESHARED_EXPORT VegetationResult {
  public:
    VegetationResult(bool hit = false);
    VegetationResult(const Vector3 &location, const Vector3 &normal, const SurfaceMaterial &material);

    inline bool isHit() const {
        return hit;
    }
    inline const Vector3 &getLocation() const {
        return location;
    }
    inline const Vector3 &getNormal() const {
        return normal;
    }
    inline const SurfaceMaterial &getMaterial() const {
        return material;
    }

  private:
    bool hit;
    Vector3 location;
    Vector3 normal;
    SurfaceMaterial material;
};
}
}
