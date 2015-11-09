#include "VegetationResult.h"

VegetationResult::VegetationResult(bool hit) : hit(hit) {
}

VegetationResult::VegetationResult(const Vector3 &location, const Vector3 &normal, const SurfaceMaterial &material)
    : location(location), normal(normal), material(material) {
    hit = true;
}
