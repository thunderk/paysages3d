#ifndef _PAYSAGES_EXPLORING_CHUNK_H_
#define _PAYSAGES_EXPLORING_CHUNK_H_

#include "rendering/tools/euclid.h"
#include "rendering/tools/color.h"
#include "rendering/renderer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ExplorerChunk ExplorerChunk;

typedef struct
{
    void (*getStatus)(ExplorerChunk* chunk);
    Vector3 (*getVertex)(ExplorerChunk* chunk, Renderer* renderer, double x, double y);
    Color (*getColor)(ExplorerChunk* chunk, Renderer* renderer, double x, double y, Vector3 vertex);
    void (*onBeforeDelete)(ExplorerChunk* chunk, void* private_data);
    void* private_data;
} ExplorerChunkOverride;

/* Public methods */
ExplorerChunk* exporerChunkCreate(Renderer* renderer, ColorProfile* color_profile, ExplorerChunkOverride override);
void explorerChunkDelete(ExplorerChunk* chunk);

/* Methods to call from an updateStatus() callback only (never to be called twice concurrently on the same chunk) */
void explorerChunkUpdateBoundingBox(ExplorerChunk* chunk, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax);
void explorerChunkMarkForReset(ExplorerChunk* chunk);

#ifdef __cplusplus
}
#endif

#endif
