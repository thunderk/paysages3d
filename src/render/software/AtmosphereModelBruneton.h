#ifndef ATMOSPHEREMODELBRUNETON_H
#define ATMOSPHEREMODELBRUNETON_H

#include "software_global.h"

#include "AtmosphereResult.h"

namespace paysages {
namespace software {

class SOFTWARESHARED_EXPORT AtmosphereModelBruneton
{
public:
    AtmosphereModelBruneton(SoftwareRenderer *parent);

    AtmosphereResult getSkyColor(Vector3 eye, const Vector3 &direction, const Vector3 &sun_position, const Color &base);
    AtmosphereResult applyAerialPerspective(Vector3 location, const Color &base);
    void fillLightingStatus(LightStatus *status, const Vector3 &normal, int opaque);

    /* Functions to get access to internal textures (for opengl shaders) */
    Texture2D* getTextureTransmittance() const;
    Texture2D* getTextureIrradiance() const;
    Texture4D* getTextureInscatter() const;

private:
    SoftwareRenderer* parent;
};

}
}

#endif // ATMOSPHEREMODELBRUNETON_H
