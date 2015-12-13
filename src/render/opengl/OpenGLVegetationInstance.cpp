#include "OpenGLVegetationInstance.h"

#include "VegetationInstance.h"

OpenGLVegetationInstance::OpenGLVegetationInstance(const VegetationInstance &wrapped) : wrapped(wrapped) {
}

void OpenGLVegetationInstance::setDistance(double distance) {
    this->distance = distance;
}
