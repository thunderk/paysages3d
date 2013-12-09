#include "BaseTestCase.h"

#include "CameraDefinition.h"
#include "SoftwareRenderer.h"
#include "AtmosphereDefinition.h"
#include "AtmosphereRenderer.h"
#include "AtmosphereResult.h"
#include "RenderingScenery.h"
#include "System.h"

#define OUTPUT_WIDTH 400
#define OUTPUT_HEIGHT 300

static Color _postProcessFragment(SoftwareRenderer* renderer, Vector3 location, void*)
{
    return renderer->getAtmosphereRenderer()->applyAerialPerspective(location, COLOR_BLACK).final;
}

TEST(Bruneton, AerialPerspective1)
{
    SoftwareRenderer renderer;
    renderer.render_width = 800;
    renderer.render_height = 600;
    renderer.render_quality = 1;

    renderer.render_camera->setLocation(VECTOR_ZERO);
    renderer.render_camera->setTarget(VECTOR_EAST);
    renderer.render_camera->setRenderSize(renderer.render_width, renderer.render_height);

    RenderArea::RenderParams params = {renderer.render_width, renderer.render_height, 1, 1};
    renderer.render_area->setParams(params);
    renderer.render_area->setBackgroundColor(COLOR_BLACK);
    renderer.render_area->clear();

    renderer.pushQuad(v3(50.0, -10.0, -50.0), v3(1.0, -10.0, -50.0), v3(1.0, -10.0, 50.0), v3(50.0, -10.0, 50.0), _postProcessFragment, NULL);
    renderer.pushQuad(v3(10.0, -10.0, -10.0), v3(10.0, -10.0, -5.0), v3(10.0, 50.0, -5.0), v3(10.0, 50.0, -10.0), _postProcessFragment, NULL);
    renderer.pushQuad(v3(15.0, -10.0, -5.0), v3(15.0, -10.0, 0.0), v3(15.0, 50.0, 0.0), v3(15.0, 50.0, -5.0), _postProcessFragment, NULL);
    renderer.pushQuad(v3(20.0, -10.0, 5.0), v3(20.0, -10.0, 10.0), v3(20.0, 50.0, 10.0), v3(20.0, 50.0, 5.0), _postProcessFragment, NULL);
    renderer.pushQuad(v3(30.0, -10.0, 25.0), v3(30.0, -10.0, 30.0), v3(30.0, 50.0, 30.0), v3(30.0, 50.0, 25.0), _postProcessFragment, NULL);
    renderer.render_area->postProcess(System::getCoreCount());

    renderer.render_area->saveToFile("./output/test_bruneton_perspective.png");
}

TEST(Bruneton, AerialPerspective2)
{
    AtmosphereDefinition* atmo = RenderingScenery::getCurrent()->getAtmosphere();
    atmo->hour = 6;
    atmo->minute = 30;
    atmo->validate();

    SoftwareRenderer renderer;
    renderer.render_width = 800;
    renderer.render_height = 600;
    renderer.render_quality = 1;

    renderer.render_camera->setLocation(VECTOR_ZERO);
    renderer.render_camera->setTarget(VECTOR_EAST);
    renderer.render_camera->setRenderSize(renderer.render_width, renderer.render_height);

    RenderArea::RenderParams params = {renderer.render_width, renderer.render_height, 1, 1};
    renderer.render_area->setParams(params);
    renderer.render_area->setBackgroundColor(COLOR_BLACK);
    renderer.render_area->clear();

    renderer.pushQuad(v3(50.0, -10.0, -50.0), v3(1.0, -10.0, -50.0), v3(1.0, -10.0, 50.0), v3(50.0, -10.0, 50.0), _postProcessFragment, NULL);
    renderer.pushQuad(v3(10.0, -10.0, -10.0), v3(10.0, -10.0, -5.0), v3(10.0, 50.0, -5.0), v3(10.0, 50.0, -10.0), _postProcessFragment, NULL);
    renderer.pushQuad(v3(15.0, -10.0, -5.0), v3(15.0, -10.0, 0.0), v3(15.0, 50.0, 0.0), v3(15.0, 50.0, -5.0), _postProcessFragment, NULL);
    renderer.pushQuad(v3(20.0, -10.0, 5.0), v3(20.0, -10.0, 10.0), v3(20.0, 50.0, 10.0), v3(20.0, 50.0, 5.0), _postProcessFragment, NULL);
    renderer.pushQuad(v3(30.0, -10.0, 25.0), v3(30.0, -10.0, 30.0), v3(30.0, 50.0, 30.0), v3(30.0, 50.0, 25.0), _postProcessFragment, NULL);
    renderer.render_area->postProcess(System::getCoreCount());

    renderer.render_area->saveToFile("./output/test_bruneton_perspective1.png");
}
