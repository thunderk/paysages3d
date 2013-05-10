#include "chunk.h"

#include <stdlib.h>

struct ExplorerChunk
{
    /* Links */
    Renderer* renderer;
    ColorProfile* color_profile;

    /* Protected override */
    ExplorerChunkOverride override;

    /* Bounding box */
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    double zmin;
    double zmax;

    /* General status */
    int reset_needed;
    double distance_to_camera;

    /* Tesselation */
    Vector3* vertex_data;
    int tessellation_max_size;
    int tessellation_current_size;
    double tessellation_step;

    /* Texture */
    /*GLuint texture_opengl_id;
    int texture_changed;
    int texture_current_size;
    int texture_max_size;*/
};

ExplorerChunk* exporerChunkCreate(Renderer* renderer, ColorProfile* color_profile, ExplorerChunkOverride override)
{
    ExplorerChunk* result;

    result = malloc(sizeof(ExplorerChunk));

    result->renderer = renderer;
    result->color_profile = color_profile;

    result->override = override;

    result->xmin = 0.0;
    result->xmax = 0.0;
    result->ymin = 0.0;
    result->ymax = 0.0;
    result->zmin = 0.0;
    result->zmax = 0.0;

    result->reset_needed = 0;
    result->distance_to_camera = 0.0;

    result->vertex_data = malloc(sizeof(Vector3) * 4);
    result->tessellation_max_size = 1;
    result->tessellation_current_size = 0;

    explorerChunkMarkForReset(result);

    /* TODO Insert in chunk list */

    return result;
}

void explorerChunkDelete(ExplorerChunk* chunk)
{
    free(chunk);
}

void explorerChunkUpdateBoundingBox(ExplorerChunk* chunk, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax)
{
    chunk->xmin = xmin;
    chunk->xmax = xmax;
    chunk->ymin = ymin;
    chunk->ymax = ymax;
    chunk->zmin = zmin;
    chunk->zmax = zmax;
}

void explorerChunkMarkForReset(ExplorerChunk* chunk)
{

}
