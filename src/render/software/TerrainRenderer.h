#ifndef TERRAINRENDERER_H
#define TERRAINRENDERER_H

#include "software_global.h"

#include "LightFilter.h"

#include "RayCastingManager.h"
#include "Color.h"

namespace paysages {
namespace software {

class SOFTWARESHARED_EXPORT TerrainRenderer: public LightFilter
{
public:
    typedef struct
    {
        Vector3 location;
        Vector3 normal;
    } TerrainResult;

public:
    TerrainRenderer(SoftwareRenderer* parent);
    virtual ~TerrainRenderer();

    virtual void update();

    virtual RayCastingResult castRay(const Vector3 &start, const Vector3 &direction);
    virtual double getHeight(double x, double z, int with_painting);
    virtual TerrainResult getResult(double x, double z, int with_painting, int with_textures);
    virtual Color getFinalColor(const Vector3 &location, double precision);
    virtual double getWaterHeight();
    virtual bool applyLightFilter(LightComponent &light, const Vector3 &at) override;

private:
    SoftwareRenderer* parent;
};

}
}

#endif // TERRAINRENDERER_H
