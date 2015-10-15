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
    LightStatus(LightingManager *manager, const Vector3 &location, const Vector3 &eye, bool filtered=true);

    inline Vector3 getLocation() const {return location;}

    void pushComponent(LightComponent component);

    Color apply(const Vector3 &normal, const SurfaceMaterial &material);

    /**
     * Return the sum of all received lights.
     */
    Color getSum() const;

    /**
     * Set the safety offset for this status.
     *
     * The offset is used to not hit the surface we are currently working on immediately.
     * It adds a small vector toward the light at the start of shadow casting.
     * This number should remain very small to not produce precision errors.
     */
    void setSafetyOffset(double safety_offset);

private:
    double safety_offset;
    double max_power;
    LightingManager* manager;
    Vector3 location;
    Vector3 eye;
    bool filtered;
    std::vector<LightComponent> components;
};

}
}

#endif // LIGHTSTATUS_H
