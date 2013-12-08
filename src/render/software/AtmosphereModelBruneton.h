#ifndef ATMOSPHEREMODELBRUNETON_H
#define ATMOSPHEREMODELBRUNETON_H

#include "software_global.h"

#include "AtmosphereResult.h"

namespace paysages {
namespace software {

class SOFTWARESHARED_EXPORT AtmosphereModelBruneton
{
public:
    AtmosphereModelBruneton(AtmosphereRenderer *parent);

    AtmosphereResult getSkyColor(const Vector3 &eye, const Vector3 &direction, const Vector3 &sun_position, const Color &base);
    AtmosphereResult applyAerialPerspective(const Vector3 &location, const Color &base);
    void fillLightingStatus(LightStatus *status, const Vector3 &normal, int opaque);

private:
    AtmosphereRenderer* parent;
};

}
}

#endif // ATMOSPHEREMODELBRUNETON_H
