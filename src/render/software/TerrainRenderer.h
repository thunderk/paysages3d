#ifndef TERRAINRENDERER_H
#define TERRAINRENDERER_H

#include "software_global.h"

#include "LightFilter.h"

#include <vector>
#include "Vector3.h"

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
    virtual TerrainResult getResult(double x, double z, bool with_painting);
    Vector3 getDisplaced(double x, double z, bool with_painting);
    virtual bool applyLightFilter(LightComponent &light, const Vector3 &at) override;

    /**
     * Estimate a probable range of altitudes, given a rectangle area.
     */
    void estimateMinMaxHeight(double x1, double z1, double x2, double z2, double *ymin, double *ymax);

    /**
     * Get the final color at a given terrain location.
     *
     * Textures will be applied (with displacement and detail), and medium traversal will be performed at the location.
     */
    virtual Color getFinalColor(double x, double z, double precision);

  private:
    SoftwareRenderer *parent;
    TerrainRayWalker *walker_ray;
    TerrainRayWalker *walker_shadows;

    bool quad_normals;
};
}
}

#endif // TERRAINRENDERER_H
