#ifndef RAYCASTINGMANAGER_H
#define RAYCASTINGMANAGER_H

#include "software_global.h"

#include "Color.h"
#include "Vector3.h"

namespace paysages {
namespace software {

typedef struct
{
    int hit;
    Color hit_color;
    Vector3 hit_location;
} RayCastingResult;
typedef RayCastingResult (*FuncGeneralCastRay)(SoftwareRenderer* renderer, Vector3 start, Vector3 direction);

class SOFTWARESHARED_EXPORT RayCastingManager
{
public:
    RayCastingManager();
};

}
}

#endif // RAYCASTINGMANAGER_H
