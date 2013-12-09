#ifndef LIGHTSTATUS_H
#define LIGHTSTATUS_H

#include "software_global.h"

#include "LightComponent.h"

namespace paysages {
namespace software {

/**
 * @brief Light status at a given point.
 *
 * The light status is the combination of all LightComponent received at a given location.
 */
class SOFTWARESHARED_EXPORT LightStatus
{
public:
    LightStatus(LightingManager *manager, const Vector3 &location, const Vector3 &eye);

    inline Vector3 getLocation() const {return location;}

    void pushComponent(LightComponent component);

    Color apply(const Vector3 &normal, const SurfaceMaterial &material);

private:
    LightingManager* manager;
    Vector3 location;
    Vector3 eye;
    std::vector<LightComponent> components;
};

}
}

#endif // LIGHTSTATUS_H