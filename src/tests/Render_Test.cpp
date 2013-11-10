#include "BaseTestCase.h"

#include <cmath>
#include "renderer.h"
#include "camera.h"
#include "System.h"

static Color _postProcessFragment(Renderer*, Vector3 location, void*)
{
    /* Checker-board */
    double x = fmod(location.x, 0.2);
    double z = fmod(location.z, 0.2);
    if (x < 0.0)
    {
        x = 0.2 + x;
    }
    if (z < 0.0)
    {
        z = 0.2 + z;
    }
    if ((x > 0.1) ^ (z > 0.1))
    {
        return COLOR_WHITE;
    }
    else
    {
        return COLOR_BLACK;
    }
}

TEST(Render, quad)
{
    Color col;
    Renderer* renderer = rendererCreate();

    renderer->render_width = 800;
    renderer->render_height = 600;
    renderer->render_quality = 1;
    renderSetToneMapping(renderer->render_area, TONE_MAPPING_CLAMP, 0.0);

    cameraSetLocationCoords(renderer->render_camera, 0.0, 0.5, 2.0);
    cameraSetTargetCoords(renderer->render_camera, 0.0, 0.5, 0.0);
    cameraSetRenderSize(renderer->render_camera, renderer->render_width, renderer->render_height);

    RenderParams params = {renderer->render_width, renderer->render_height, 1, 1};
    renderSetParams(renderer->render_area, params);

    renderSetBackgroundColor(renderer->render_area, &COLOR_BLUE);
    renderClear(renderer->render_area);

    renderer->pushQuad(renderer, v3(-1.0, 0.0, 1.0), v3(-1.0, 0.0, -1.0), v3(1.0, 0.0, -1.0), v3(1.0, 0.0, 1.0), _postProcessFragment, NULL);
    renderPostProcess(renderer->render_area, System::getCoreCount());

    col = renderGetPixel(renderer->render_area, 399, 599 - 435);
    ASSERT_COLOR_RGBA(col, 1.0, 1.0, 1.0, 1.0);
    col = renderGetPixel(renderer->render_area, 399, 599 - 436);
    ASSERT_COLOR_RGBA(col, 0.0, 0.0, 0.0, 1.0);
    col = renderGetPixel(renderer->render_area, 400, 599 - 435);
    ASSERT_COLOR_RGBA(col, 0.0, 0.0, 0.0, 1.0);
    col = renderGetPixel(renderer->render_area, 400, 599 - 436);
    ASSERT_COLOR_RGBA(col, 1.0, 1.0, 1.0, 1.0);

    renderSaveToFile(renderer->render_area, "./output/test_render_quad.png");

    rendererDelete(renderer);
}
