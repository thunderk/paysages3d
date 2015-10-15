#ifndef RAYCASTINGMANAGER_H
#define RAYCASTINGMANAGER_H

#include "software_global.h"

namespace paysages {
namespace software {

typedef RayCastingResult (*FuncGeneralCastRay)(SoftwareRenderer* renderer, const Vector3 &start, const Vector3 &direction);

class SOFTWARESHARED_EXPORT RayCastingManager
{
public:
    RayCastingManager();
};

}
}

#endif // RAYCASTINGMANAGER_H
