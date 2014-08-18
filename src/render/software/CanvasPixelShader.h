#ifndef CANVASPIXELSHADER_H
#define CANVASPIXELSHADER_H

#include "software_global.h"

#include "ParallelWorker.h"

namespace paysages {
namespace software {

/**
 * @brief Parallel worker that can work on canvas portion to resolve pixel colors.
 *
 * This is used after the rasterization phase to compute pixel colors from the fragments stored in them.
 *
 * This worker will be set to work on a given chunk of a canvas portion.
 */
class CanvasPixelShader: public ParallelWorker
{
public:
    CanvasPixelShader(const SoftwareCanvasRenderer &renderer, CanvasPortion *portion, int chunk_size, int sub_chunk_size, int chunks_x, int chunks_y);

    virtual int processParallelUnit(int unit);

private:
    const SoftwareCanvasRenderer &renderer;
    CanvasPortion *portion;
    int chunk_size;
    int sub_chunk_size;
    int chunks_x;
    int chunks_y;
};

}
}

#endif // CANVASPIXELSHADER_H
