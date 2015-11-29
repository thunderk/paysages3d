#ifndef OPENGLVEGETATIONINSTANCE_H
#define OPENGLVEGETATIONINSTANCE_H

#include "opengl_global.h"

#include "VegetationInstance.h"

namespace paysages {
namespace opengl {

/**
 * A single instance of vegetation.
 */
class OPENGLSHARED_EXPORT OpenGLVegetationInstance {
  public:
    OpenGLVegetationInstance(const VegetationInstance &wrapped);

    inline const VegetationModelDefinition &getModel() const {
        return wrapped.getModel();
    }
    inline const Vector3 &getBase() const {
        return wrapped.getBase();
    }
    inline double getSize() const {
        return wrapped.getSize();
    }
    inline double getDistance() const {
        return distance;
    }

    /**
     * Set the distance to camera, mainly for sorting.
     */
    void setDistance(double distance);

  private:
    VegetationInstance wrapped;
    double distance;
};
}
}

#endif // OPENGLVEGETATIONINSTANCE_H
