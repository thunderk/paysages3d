#pragma once

#include "software_global.h"

#include "LightSource.h"

namespace paysages {
namespace software {

class BaseAtmosphereRenderer : public LightSource {
  public:
    BaseAtmosphereRenderer(SoftwareRenderer *parent);
    virtual ~BaseAtmosphereRenderer() {
    }

    virtual AtmosphereResult applyAerialPerspective(const Vector3 &location, const Color &base);
    virtual AtmosphereResult getSkyColor(const Vector3 &direction);
    virtual Vector3 getSunDirection() const;
    virtual Vector3 getSunLocation() const;

    virtual bool getLightsAt(vector<LightComponent> &result, const Vector3 &location) const override;

  protected:
    virtual AtmosphereDefinition *getDefinition() const;
    SoftwareRenderer *parent;
};

class SoftwareBrunetonAtmosphereRenderer : public BaseAtmosphereRenderer {
  public:
    SoftwareBrunetonAtmosphereRenderer(SoftwareRenderer *parent);
    virtual ~SoftwareBrunetonAtmosphereRenderer();

    virtual AtmosphereResult applyAerialPerspective(const Vector3 &location, const Color &base) override;
    virtual AtmosphereResult getSkyColor(const Vector3 &direction) override;

    virtual bool getLightsAt(vector<LightComponent> &result, const Vector3 &location) const override;

    inline const AtmosphereModelBruneton *getModel() const {
        return model;
    }

  private:
    AtmosphereModelBruneton *model;
};
}
}
