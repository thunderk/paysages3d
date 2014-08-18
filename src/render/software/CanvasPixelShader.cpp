#include "CanvasPixelShader.h"

#include "Color.h"
#include "SoftwareCanvasRenderer.h"
#include "CanvasPortion.h"
#include "CanvasPixel.h"
#include "CanvasFragment.h"
#include "Rasterizer.h"

CanvasPixelShader::CanvasPixelShader(const SoftwareCanvasRenderer &renderer, CanvasPortion *portion, int chunk_size, int sub_chunk_size, int chunks_x, int chunks_y):
    renderer(renderer), portion(portion), chunk_size(chunk_size), sub_chunk_size(sub_chunk_size), chunks_x(chunks_x), chunks_y(chunks_y)
{
}

int CanvasPixelShader::processParallelUnit(int unit)
{
    // Locate the chunk we work on
    int prev_sub_chunk_size = chunk_size * 2;
    int chunk_x = unit % chunks_x;
    int chunk_y = unit / chunks_x;
    int base_x = chunk_x * chunk_size;
    int base_y = chunk_y * chunk_size;
    int limit_x = portion->getWidth() - base_x;
    int limit_y = portion->getHeight() - base_y;

    limit_x = (limit_x > chunk_size) ? chunk_size : limit_x;
    limit_y = (limit_y > chunk_size) ? chunk_size : limit_y;

    for (int x = 0; x < limit_x; x += sub_chunk_size)
    {
        for (int y = 0; y < limit_y; y += sub_chunk_size)
        {
            if (x % prev_sub_chunk_size != 0 or y % prev_sub_chunk_size != 0)
            {
                // Resolve the pixel color
                const CanvasPixel &pixel = portion->at(base_x + x, base_y + y);
                int n = pixel.getFragmentCount();
                Color composite = COLOR_BLACK;
                for (int i = 0; i < n; i++)
                {
                    const CanvasFragment &fragment = pixel.getFragment(i);
                    const Rasterizer &rasterizer = renderer.getRasterizer(fragment.getClient());
                    composite.mask(rasterizer.shadeFragment(fragment));
                }
                portion->setColor(base_x + x, base_y + y, composite);
            }
        }
    }

    return 0;
}
