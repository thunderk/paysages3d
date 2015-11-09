#include "VegetationInstance.h"

VegetationInstance::VegetationInstance(const VegetationModelDefinition &model, const Vector3 &base, double size,
                                       double angle)
    : model(model), base(base), size(size), angle(angle) {
}

VegetationInstance VegetationInstance::displace(const Vector3 &location, const Vector3 &normal) const {
    return VegetationInstance(model, location, size, angle);
}
