#ifndef VEGETATIONINSTANCE_H
#define VEGETATIONINSTANCE_H

#include "definition_global.h"

#include "Vector3.h"

namespace paysages {
namespace definition {

/**
 * Single instance of a vegetation layer (e.g. a single tree).
 *
 * This is used as potential hit on vegetation lookup.
 */
class DEFINITIONSHARED_EXPORT VegetationInstance {
  public:
    VegetationInstance(const VegetationModelDefinition &model, const Vector3 &base, double size = 1.0,
                       double angle = 0.0);

    inline const VegetationModelDefinition &getModel() const {
        return model;
    }
    inline const Vector3 &getBase() const {
        return base;
    }
    inline double getSize() const {
        return size;
    }

    /**
     * Return a copy of this instance, with terrain displacement applied.
     */
    VegetationInstance displace(const Vector3 &location, const Vector3 &normal) const;

  private:
    const VegetationModelDefinition &model;
    Vector3 base;
    double size;
    double angle;
};
}
}

#endif // VEGETATIONINSTANCE_H
