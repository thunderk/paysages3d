#ifndef _RENDERING_TERRAIN_RASTER_H_
#define _RENDERING_TERRAIN_RASTER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    Vector3 point_nw;
    Vector3 point_sw;
    Vector3 point_se;
    Vector3 point_ne;
    int detail_hint;
} TerrainChunkInfo;

typedef int (*FuncTerrainTessellationCallback)(Renderer* renderer, TerrainChunkInfo* chunk, double progress);

/**
 * Tessellate the terrain, yielding chunks information.
 *
 * The terrain will be broken in chunks, most detailed near the camera.
 */
void terrainGetTessellationInfo(Renderer* renderer, FuncTerrainTessellationCallback callback, int displaced);

/**
 * Tessellate a terrain chunk, pushing the quads in the render area.
 */
void terrainTessellateChunk(Renderer* renderer, TerrainChunkInfo* chunk, int detail);

/**
 * Start the final rasterization of terrain.
 *
 * This will push the rasterized quads in the render area, waiting for post process.
 */
void terrainRenderSurface(Renderer* renderer);

#ifdef __cplusplus
}
#endif

#endif
