#pragma once

#include "software_global.h"

#include "Rasterizer.h"
#include "Vector3.h"

namespace paysages {
namespace software {

class SOFTWARESHARED_EXPORT TerrainRasterizer : public Rasterizer {
  public:
    typedef struct {
        Vector3 point_nw;
        Vector3 point_sw;
        Vector3 point_se;
        Vector3 point_ne;
        int detail_hint;
    } TerrainChunkInfo;

  public:
    TerrainRasterizer(SoftwareRenderer *renderer, RenderProgress *progress, unsigned short client_id);

    /**
     * Set the rasterization quality.
     *
     * base_chunk_size - Size of chunks near the camera
     * detail_factor - Precision factor of a chunk's tessellation, depending on screen coverage
     * max_chunk_detail - Maximal tessellation of chunks
     */
    void setQuality(double base_chunk_size, double detail_factor, int max_chunk_detail);
    virtual void setQuality(double factor) override;

    virtual int prepareRasterization() override;
    virtual void rasterizeToCanvas(CanvasPortion *canvas) override;
    virtual Color shadeFragment(const CanvasFragment &fragment, const CanvasFragment *previous) const override;

  protected:
    /**
     * Add a vertical offset to rasterized polygons.
     *
     * This may be used to rasterize a covering layer on top of ground.
     */
    void setYOffset(double offset);

  private:
    /**
     * Method called for each chunk tessellated by performTessellation.
     */
    void processChunk(CanvasPortion *canvas, TerrainChunkInfo *chunk);

    /**
     * Tessellate the terrain, calling processChunk for each chunk.
     *
     * The terrain will be broken in chunks, most detailed near the camera.
     *
     * Return the number of quads that has been pushed.
     *
     * 'canvas' may be NULL to only simulate the tessellation.
     */
    int performTessellation(CanvasPortion *canvas);

    /**
     * Tessellate a terrain chunk, pushing the quads in the render area.
     */
    void tessellateChunk(CanvasPortion *canvas, TerrainChunkInfo *chunk, int detail);

    void renderQuad(CanvasPortion *canvas, double x, double z, double size, double water_height);

    void getChunk(SoftwareRenderer *renderer, TerrainRasterizer::TerrainChunkInfo *chunk, double x, double z,
                  double size);

  private:
    double yoffset;

    // Quality control
    double base_chunk_size;
    double detail_factor;
    int max_chunk_detail;
};
}
}
