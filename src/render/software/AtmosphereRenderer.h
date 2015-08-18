#ifndef ATMOSPHERERENDERER_H
#define ATMOSPHERERENDERER_H

#include "software_global.h"

#include "Color.h"
#include "LightComponent.h"

namespace paysages {
namespace software {

class BaseAtmosphereRenderer
{
public:
    BaseAtmosphereRenderer(SoftwareRenderer* parent);
    virtual ~BaseAtmosphereRenderer() {}

    virtual void getLightingStatus(LightStatus* status, Vector3 normal, int opaque);
    virtual AtmosphereResult applyAerialPerspective(Vector3 location, Color base);
    virtual AtmosphereResult getSkyColor(Vector3 direction);
    virtual Vector3 getSunDirection(bool cache=true) const;

    void setBasicLights();
    void setStaticLights(const std::vector<LightComponent> &lights);

protected:
    virtual AtmosphereDefinition* getDefinition() const;
    SoftwareRenderer* parent;
    std::vector<LightComponent> lights;
};

class SoftwareBrunetonAtmosphereRenderer: public BaseAtmosphereRenderer
{
public:
    SoftwareBrunetonAtmosphereRenderer(SoftwareRenderer* parent);
    virtual ~SoftwareBrunetonAtmosphereRenderer();

    virtual void getLightingStatus(LightStatus* status, Vector3 normal, int opaque) override;
    virtual AtmosphereResult applyAerialPerspective(Vector3 location, Color base) override;
    virtual AtmosphereResult getSkyColor(Vector3 direction) override;

    inline const AtmosphereModelBruneton* getModel() const {return model;}

private:
    AtmosphereModelBruneton* model;
};

}
}

#endif // ATMOSPHERERENDERER_H
