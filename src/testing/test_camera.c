#include "testing/common.h"

#include "rendering/camera.h"

START_TEST(test_camera_definition)
{
    CameraDefinition* cam;
    cam = cameraCreateDefinition();

    cameraSetLocationCoords(cam, 0.0, 1.0, 1.0);
    cameraSetTargetCoords(cam, 0.0, 0.0, 0.0);

    ck_assert_vector_values(cameraGetLocation(cam), 0.0, 1.0, 1.0);
    ck_assert_vector_values(cameraGetTarget(cam), 0.0, 0.0, 0.0);

    cameraDeleteDefinition(cam);
}
END_TEST

TEST_CASE(camera, test_camera_definition)

