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
    TerrainRasterizer(SoftwareRenderer* renderer, int client_id);

    /**
     * Method called for each chunk tessellated by getTessellationInfo.
     */
    int processChunk(CanvasPortion* canvas, TerrainChunkInfo* chunk, double progress);

    /**
     * Tessellate the terrain, calling processChunk for each chunk.
     *
     * The terrain will be broken in chunks, most detailed near the camera.
     */
    void getTessellationInfo(CanvasPortion* canvas, int displaced);

    /**
     * Tessellate a terrain chunk, pushing the quads in the render area.
     */
    void tessellateChunk(CanvasPortion* canvas, TerrainChunkInfo* chunk, int detail);

    void renderQuad(CanvasPortion* canvas, double x, double z, double size, double water_height);

    virtual void rasterizeToCanvas(CanvasPortion* canvas) override;
    virtual Color shadeFragment(const CanvasFragment &fragment) const override;

private:
    ParallelQueue* queue;
};

}
}

#endif // TERRAINRASTERIZER_H
