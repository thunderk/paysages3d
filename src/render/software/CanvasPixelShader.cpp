#include "CanvasPixelShader.h"

#include "CanvasFragment.h"
#include "CanvasPixel.h"
#include "CanvasPortion.h"
#include "Color.h"
#include "Rasterizer.h"
#include "RenderProgress.h"
#include "SoftwareCanvasRenderer.h"

CanvasPixelShader::CanvasPixelShader(const SoftwareCanvasRenderer &renderer, CanvasPortion *portion,
                                     RenderProgress *progress, int chunk_size, int sub_chunk_size, int chunks_x,
                                     int chunks_y)
    : renderer(renderer), portion(portion), progress(progress), chunk_size(chunk_size), sub_chunk_size(sub_chunk_size),
      chunks_x(chunks_x), chunks_y(chunks_y) {
}

void CanvasPixelShader::processParallelUnit(int unit) {
    // Locate the chunk we work on
    int prev_sub_chunk_size = sub_chunk_size * 2;
    int chunk_x = unit / chunks_y;
    int chunk_y = unit % chunks_y;
    int base_x = chunk_x * chunk_size;
    int base_y = chunk_y * chunk_size;
    int limit_x = portion->getWidth() - base_x;
    int limit_y = portion->getHeight() - base_y;

    limit_x = (limit_x > chunk_size) ? chunk_size : limit_x;
    limit_y = (limit_y > chunk_size) ? chunk_size : limit_y;

    // Iterate on sub-chunks
    for (int x = 0; x < limit_x; x += sub_chunk_size) {
        int done = 0;
        for (int y = 0; y < limit_y; y += sub_chunk_size) {
            if (interrupted) {
                return;
            }

            if (sub_chunk_size == chunk_size or x % prev_sub_chunk_size != 0 or y % prev_sub_chunk_size != 0) {
                // Resolve the pixel color
                const CanvasPixel &pixel = portion->at(base_x + x, base_y + y);
                int n = pixel.getFragmentCount();
                Color composite = COLOR_BLACK;
                const CanvasFragment *previous = NULL;
                for (int i = 0; i < n; i++) {
                    const CanvasFragment &fragment = pixel.getFragment(i);
                    const Rasterizer &rasterizer = renderer.getRasterizer(fragment.getClient());
                    composite.mask(rasterizer.shadeFragment(fragment, previous));
                    previous = &fragment;
                }

                // Fill the square area
                for (int fx = 0; fx + x < limit_x and fx < sub_chunk_size; fx++) {
                    for (int fy = 0; fy + y < limit_y and fy < sub_chunk_size; fy++) {
                        portion->setColor(base_x + x + fx, base_y + y + fy, composite);
                    }
                }

                done++;
            }
        }
        progress->add(done);
    }
}
