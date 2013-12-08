#ifndef ATMOSPHERERENDERER_H
#define ATMOSPHERERENDERER_H

#include "software_global.h"

#include "Color.h"

namespace paysages {
namespace software {

class BaseAtmosphereRenderer
{
public:
    BaseAtmosphereRenderer(SoftwareRenderer* renderer);
    virtual ~BaseAtmosphereRenderer() {}

    virtual void getLightingStatus(LightStatus* status, Vector3 normal, int opaque);
    virtual AtmosphereResult applyAerialPerspective(Vector3 location, Color base);
    virtual AtmosphereResult getSkyColor(Vector3 direction);
    virtual Vector3 getSunDirection();

protected:
    virtual AtmosphereDefinition* getDefinition();
    SoftwareRenderer* renderer;
};

class SoftwareBrunetonAtmosphereRenderer: public BaseAtmosphereRenderer
{
public:
    SoftwareBrunetonAtmosphereRenderer(SoftwareRenderer* renderer);
    virtual ~SoftwareBrunetonAtmosphereRenderer();

    virtual void getLightingStatus(LightStatus* status, Vector3 normal, int opaque) override;
    virtual AtmosphereResult applyAerialPerspective(Vector3 location, Color base) override;
    virtual AtmosphereResult getSkyColor(Vector3 direction) override;

private:
    AtmosphereModelBruneton* model;
};

}
}

#endif // ATMOSPHERERENDERER_H
