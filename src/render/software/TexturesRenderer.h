#ifndef TEXTURESRENDERER_H
#define TEXTURESRENDERER_H

#include "software_global.h"

#include "TerrainRenderer.h"

#define TEXTURES_MAX_LAYERS 50

namespace paysages {
namespace software {

class SOFTWARESHARED_EXPORT TexturesRenderer {
  public:
    typedef struct {
        TextureLayerDefinition *definition;
        double presence;
        Color color;
    } TexturesLayerResult;

    typedef struct {
        Vector3 base_location;
        Vector3 base_normal;
        int layer_count;
        TexturesLayerResult layers[TEXTURES_MAX_LAYERS];
        Vector3 final_location;
        Color final_color;
    } TexturesResult;

  public:
    TexturesRenderer(SoftwareRenderer *parent);
    virtual ~TexturesRenderer();

    virtual void update();

    virtual double getMaximalDisplacement(TexturesDefinition *textures);
    virtual double getLayerBasePresence(TextureLayerDefinition *layer, const TerrainRenderer::TerrainResult &terrain);
    virtual double getTriplanarNoise(const FractalNoise *noise, const Vector3 &location, const Vector3 &normal, double detail);

    virtual Vector3 displaceTerrain(const TerrainRenderer::TerrainResult &terrain);
    virtual double getBasePresence(int layer, const TerrainRenderer::TerrainResult &terrain);
    virtual TexturesResult applyToTerrain(double x, double z, double precision);

  private:
    SoftwareRenderer *parent;
};
}
}

#endif // TEXTURESRENDERER_H
