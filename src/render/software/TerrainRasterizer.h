#ifndef TERRAINRASTERIZER_H
#define TERRAINRASTERIZER_H

#include "software_global.h"

#include "Vector3.h"

namespace paysages {
namespace software {

class SOFTWARESHARED_EXPORT TerrainRasterizer
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
    TerrainRasterizer(SoftwareRenderer* renderer);

    /**
     * Method called for each chunk tessellated by getTessellationInfo.
     */
    int processChunk(TerrainChunkInfo* chunk, double progress);

    /**
     * Tessellate the terrain, calling processChunk for each chunk.
     *
     * The terrain will be broken in chunks, most detailed near the camera.
     */
    void getTessellationInfo(int displaced);

    /**
     * Tessellate a terrain chunk, pushing the quads in the render area.
     */
    void tessellateChunk(TerrainChunkInfo* chunk, int detail);

    /**
     * Start the final rasterization of terrain.
     *
     * This will push the rasterized quads in the render area, waiting for post process.
     */
    void renderSurface();

private:
    SoftwareRenderer* renderer;
};

}
}

#endif // TERRAINRASTERIZER_H
