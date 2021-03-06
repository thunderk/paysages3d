#pragma once

#include "software_global.h"

#include "LightSource.h"

namespace paysages {
namespace software {

class SOFTWARESHARED_EXPORT AtmosphereModelBruneton : public LightSource {
  public:
    AtmosphereModelBruneton(SoftwareRenderer *parent);
    virtual ~AtmosphereModelBruneton();

    AtmosphereResult getSkyColor(Vector3 eye, const Vector3 &direction, const Vector3 &sun_position,
                                 const Color &base) const;
    AtmosphereResult applyAerialPerspective(Vector3 location, const Color &base) const;
    virtual bool getLightsAt(vector<LightComponent> &result, const Vector3 &location) const override;

    /* Functions to get access to internal textures (for opengl shaders) */
    Texture2D *getTextureTransmittance() const;
    Texture2D *getTextureIrradiance() const;
    Texture4D *getTextureInscatter() const;

  private:
    SoftwareRenderer *parent;
};
}
}
