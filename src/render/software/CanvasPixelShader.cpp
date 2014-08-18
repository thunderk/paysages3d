#include "CanvasPixelShader.h"

#include "Color.h"
#include "SoftwareCanvasRenderer.h"
#include "CanvasPortion.h"
#include "CanvasPixel.h"
#include "CanvasFragment.h"
#include "Rasterizer.h"

CanvasPixelShader::CanvasPixelShader(const SoftwareCanvasRenderer &renderer, CanvasPortion *portion, int chunk_size, int chunks_x, int chunks_y):
    renderer(renderer), portion(portion), chunk_size(chunk_size), chunks_x(chunks_x), chunks_y(chunks_y)
{
}

int CanvasPixelShader::processParallelUnit(int unit)
{
    // Locate the chunk we work on
    int chunk_x = unit % chunks_x;
    int chunk_y = unit / chunks_x;

    // Resolve the pixel color
    int x = chunk_x * chunk_size;
    int y = chunk_y * chunk_size;
    const CanvasPixel &pixel = portion->at(x, y);
    int n = pixel.getFragmentCount();
    Color composite = COLOR_BLACK;
    for (int i = 0; i < n; i++)
    {
        const CanvasFragment &fragment = pixel.getFragment(i);
        const Rasterizer &rasterizer = renderer.getRasterizer(fragment.getClient());
        composite.mask(rasterizer.shadeFragment(fragment));
    }
    portion->setColor(x, y, composite);

    return 0;
}
