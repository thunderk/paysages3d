#ifndef TERRAINRENDERER_H
#define TERRAINRENDERER_H

#include "software_global.h"

#include "RayCastingManager.h"
#include "Color.h"

namespace paysages {
namespace software {

class SOFTWARESHARED_EXPORT TerrainRenderer
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

    virtual RayCastingResult castRay(const Vector3 &start, const Vector3 &direction);
    virtual double getHeight(double x, double z, int with_painting);
    virtual TerrainResult getResult(double x, double z, int with_painting, int with_textures);
    virtual Color getFinalColor(const Vector3 &location, double precision);
    virtual double getWaterHeight();
    virtual int alterLight(LightComponent *light, const Vector3 &location);

private:
    SoftwareRenderer* parent;
};

}
}

#endif // TERRAINRENDERER_H
