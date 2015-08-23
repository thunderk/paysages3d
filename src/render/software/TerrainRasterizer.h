#ifndef TERRAINRASTERIZER_H
#define TERRAINRASTERIZER_H

#include "software_global.h"

#include "Rasterizer.h"
#include "Vector3.h"

namespace paysages {
namespace software {

class SOFTWARESHARED_EXPORT TerrainRasterizer: public Rasterizer
{
public:
    typedef struct
    {
        Vector3 point_nw;
        Vector3 point_sw;
        Vector3 point_se;
        Vector3 point_ne;
        int detail_hint;
    } TerrainChunkInfo;

public:
    TerrainRasterizer(SoftwareRenderer* renderer, RenderProgress *progress, int client_id);

    virtual int prepareRasterization() override;
    virtual void rasterizeToCanvas(CanvasPortion* canvas) override;
    virtual Color shadeFragment(const CanvasFragment &fragment) const override;

private:
    /**
     * Method called for each chunk tessellated by performTessellation.
     */
    void processChunk(CanvasPortion* canvas, TerrainChunkInfo* chunk);

    /**
     * Tessellate the terrain, calling processChunk for each chunk.
     *
     * The terrain will be broken in chunks, most detailed near the camera.
     *
     * Return the number of quads that has been pushed.
     *
     * *canvas* may be NULL to only simulate the tessellation.
     */
    int performTessellation(CanvasPortion* canvas, bool displaced);

    /**
     * Tessellate a terrain chunk, pushing the quads in the render area.
     */
    void tessellateChunk(CanvasPortion* canvas, TerrainChunkInfo* chunk, int detail);

    void renderQuad(CanvasPortion* canvas, double x, double z, double size, double water_height);
};

}
}

#endif // TERRAINRASTERIZER_H
