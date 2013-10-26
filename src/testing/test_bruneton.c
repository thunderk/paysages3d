#include "testing/common.h"
#include "System.h"
#include "rendering/scenery.h"

#define OUTPUT_WIDTH 400
#define OUTPUT_HEIGHT 300

static Color _postProcessFragment(Renderer* renderer, Vector3 location, void* data)
{
    return renderer->atmosphere->applyAerialPerspective(renderer, location, COLOR_BLACK).final;
}

START_TEST(testBrunetonAerialPerspective)
{
    Renderer* renderer = sceneryCreateStandardRenderer();
    renderer->render_width = 800;
    renderer->render_height = 600;
    renderer->render_quality = 1;

    cameraSetLocation(renderer->render_camera, VECTOR_ZERO);
    cameraSetTarget(renderer->render_camera, VECTOR_EAST);
    cameraSetRenderSize(renderer->render_camera, renderer->render_width, renderer->render_height);

    RenderParams params = {renderer->render_width, renderer->render_height, 1, 1};
    renderSetParams(renderer->render_area, params);
    renderSetBackgroundColor(renderer->render_area, &COLOR_BLACK);
    renderClear(renderer->render_area);

    renderer->pushQuad(renderer, v3(50.0, -10.0, -50.0), v3(1.0, -10.0, -50.0), v3(1.0, -10.0, 50.0), v3(50.0, -10.0, 50.0), _postProcessFragment, NULL);
    renderer->pushQuad(renderer, v3(10.0, -10.0, -10.0), v3(10.0, -10.0, -5.0), v3(10.0, 50.0, -5.0), v3(10.0, 50.0, -10.0), _postProcessFragment, NULL);
    renderer->pushQuad(renderer, v3(15.0, -10.0, -5.0), v3(15.0, -10.0, 0.0), v3(15.0, 50.0, 0.0), v3(15.0, 50.0, -5.0), _postProcessFragment, NULL);
    renderer->pushQuad(renderer, v3(20.0, -10.0, 5.0), v3(20.0, -10.0, 10.0), v3(20.0, 50.0, 10.0), v3(20.0, 50.0, 5.0), _postProcessFragment, NULL);
    renderer->pushQuad(renderer, v3(30.0, -10.0, 25.0), v3(30.0, -10.0, 30.0), v3(30.0, 50.0, 30.0), v3(30.0, 50.0, 25.0), _postProcessFragment, NULL);
    renderPostProcess(renderer->render_area, tests_cpu_count);

    renderSaveToFile(renderer->render_area, "./output/test_bruneton_perspective.png");

    rendererDelete(renderer);
}
END_TEST

START_TEST(testBrunetonAerialPerspective1)
{
    AtmosphereDefinition* atmo = AtmosphereDefinitionClass.create();
    sceneryGetAtmosphere(atmo);
    atmo->hour = 6;
    atmo->minute = 30;
    scenerySetAtmosphere(atmo);
    AtmosphereDefinitionClass.destroy(atmo);

    Renderer* renderer = sceneryCreateStandardRenderer();
    renderer->render_width = 800;
    renderer->render_height = 600;
    renderer->render_quality = 1;

    cameraSetLocation(renderer->render_camera, VECTOR_ZERO);
    cameraSetTarget(renderer->render_camera, VECTOR_EAST);
    cameraSetRenderSize(renderer->render_camera, renderer->render_width, renderer->render_height);

    RenderParams params = {renderer->render_width, renderer->render_height, 1, 1};
    renderSetParams(renderer->render_area, params);
    renderSetBackgroundColor(renderer->render_area, &COLOR_BLACK);
    renderClear(renderer->render_area);

    renderer->pushQuad(renderer, v3(50.0, -10.0, -50.0), v3(1.0, -10.0, -50.0), v3(1.0, -10.0, 50.0), v3(50.0, -10.0, 50.0), _postProcessFragment, NULL);
    renderer->pushQuad(renderer, v3(10.0, -10.0, -10.0), v3(10.0, -10.0, -5.0), v3(10.0, 50.0, -5.0), v3(10.0, 50.0, -10.0), _postProcessFragment, NULL);
    renderer->pushQuad(renderer, v3(15.0, -10.0, -5.0), v3(15.0, -10.0, 0.0), v3(15.0, 50.0, 0.0), v3(15.0, 50.0, -5.0), _postProcessFragment, NULL);
    renderer->pushQuad(renderer, v3(20.0, -10.0, 5.0), v3(20.0, -10.0, 10.0), v3(20.0, 50.0, 10.0), v3(20.0, 50.0, 5.0), _postProcessFragment, NULL);
    renderer->pushQuad(renderer, v3(30.0, -10.0, 25.0), v3(30.0, -10.0, 30.0), v3(30.0, 50.0, 30.0), v3(30.0, 50.0, 25.0), _postProcessFragment, NULL);
    renderPostProcess(renderer->render_area, tests_cpu_count);

    renderSaveToFile(renderer->render_area, "./output/test_bruneton_perspective1.png");

    rendererDelete(renderer);
}
END_TEST

TEST_CASE(bruneton,
          testBrunetonAerialPerspective,
          testBrunetonAerialPerspective1)

