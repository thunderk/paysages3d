#include "BaseTestCase.h"

#include <cmath>
#include "SoftwareRenderer.h"
#include "CameraDefinition.h"
#include "ColorProfile.h"
#include "System.h"

static Color _postProcessFragment(SoftwareRenderer*, Vector3 location, void*)
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

static void _render_quad_checker(SoftwareRenderer &renderer)
{
    renderer.render_width = 800;
    renderer.render_height = 600;
    renderer.render_quality = 1;
    renderer.render_area->setToneMapping(ColorProfile(ColorProfile::TONE_MAPPING_CLAMP, 0.0));

    renderer.render_camera->setRenderSize(renderer.render_width, renderer.render_height);

    RenderArea::RenderParams params = {renderer.render_width, renderer.render_height, 1, 1};
    renderer.render_area->setParams(params);

    renderer.render_area->setBackgroundColor(COLOR_BLUE);
    renderer.render_area->clear();

    renderer.pushQuad(Vector3(-1.0, 0.0, 1.0), Vector3(-1.0, 0.0, -1.0), Vector3(1.0, 0.0, -1.0), Vector3(1.0, 0.0, 1.0), _postProcessFragment, NULL);
    renderer.render_area->postProcess(System::getCoreCount());
}

TEST(Render, quad)
{
    SoftwareRenderer renderer;

    renderer.render_camera->setLocationCoords(0.0, 0.5, 2.0);
    renderer.render_camera->setTargetCoords(0.0, 0.5, 0.0);

    _render_quad_checker(renderer);

    Color col;
    col = renderer.render_area->getPixel(399, 599 - 435);
    ASSERT_COLOR_RGBA(col, 1.0, 1.0, 1.0, 1.0);
    col = renderer.render_area->getPixel(399, 599 - 436);
    ASSERT_COLOR_RGBA(col, 0.0, 0.0, 0.0, 1.0);
    col = renderer.render_area->getPixel(400, 599 - 435);
    ASSERT_COLOR_RGBA(col, 0.0, 0.0, 0.0, 1.0);
    col = renderer.render_area->getPixel(400, 599 - 436);
    ASSERT_COLOR_RGBA(col, 1.0, 1.0, 1.0, 1.0);

    renderer.render_area->saveToFile("./output/test_render_quad.png");
}

TEST(Render, quad_cut)
{
    SoftwareRenderer renderer;

    renderer.render_camera->setLocationCoords(0.8, 0.7, 1.0);
    renderer.render_camera->setTargetCoords(0.0, 0.0, -0.5);

    _render_quad_checker(renderer);

    renderer.render_area->saveToFile("./output/test_render_quad_cut.png");
}
