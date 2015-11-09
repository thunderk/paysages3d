#ifndef TERRAINRENDERER_H
#define TERRAINRENDERER_H

#include "software_global.h"

#include "LightFilter.h"

#include "Vector3.h"
#include "Color.h"

namespace paysages {
namespace software {

class SOFTWARESHARED_EXPORT TerrainRenderer : public LightFilter {
  public:
    typedef struct {
        Vector3 location;
        Vector3 normal;
    } TerrainResult;

  public:
    TerrainRenderer(SoftwareRenderer *parent);
    virtual ~TerrainRenderer();

    virtual void update();
    void setQuality(bool quad_normals, double ray_precision, double shadow_precision);
    void setQuality(double factor);

    virtual RayCastingResult castRay(const Vector3 &start, const Vector3 &direction);
    virtual double getHeight(double x, double z, bool with_painting, bool water_offset = true);
    virtual TerrainResult getResult(double x, double z, bool with_painting, bool with_textures);
    virtual Color getFinalColor(const Vector3 &location, double precision);
    virtual bool applyLightFilter(LightComponent &light, const Vector3 &at) override;

  private:
    SoftwareRenderer *parent;
    TerrainRayWalker *walker_ray;
    TerrainRayWalker *walker_shadows;

    bool quad_normals;
};
}
}

#endif // TERRAINRENDERER_H
