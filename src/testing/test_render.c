#include "testing/common.h"

#include <math.h>
#include "rendering/renderer.h"
#include "rendering/tools.h"

static Color _postProcessFragment(Renderer* renderer, Vector3 location, void* data)
{
    UNUSED(renderer);
    UNUSED(data);

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

START_TEST(test_render_quad)
{
    Renderer* renderer = rendererCreate();

    renderer->render_width = 800;
    renderer->render_height = 600;
    renderer->render_quality = 1;

    cameraSetLocationCoords(renderer->render_camera, 0.0, 0.5, 2.0);
    cameraSetTargetCoords(renderer->render_camera, 0.0, 0.5, 0.0);
    cameraSetRenderSize(renderer->render_camera, renderer->render_width, renderer->render_height);

    RenderParams params = {renderer->render_width, renderer->render_height, 1, 1};
    renderSetParams(renderer->render_area, params);

    renderSetBackgroundColor(renderer->render_area, &COLOR_BLUE);
    renderClear(renderer->render_area);

    renderer->pushQuad(renderer, v3(-1.0, 0.0, 1.0), v3(-1.0, 0.0, -1.0), v3(1.0, 0.0, -1.0), v3(1.0, 0.0, 1.0), _postProcessFragment, NULL);
    renderPostProcess(renderer->render_area, renderer, tests_cpu_count);

    renderSaveToFile(renderer->render_area, "./output/test_render_quad.png");

    rendererDelete(renderer);
}
END_TEST

TEST_CASE(render, test_render_quad)

