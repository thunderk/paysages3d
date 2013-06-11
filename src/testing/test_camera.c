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

START_TEST(test_camera_projection)
{
    CameraDefinition* cam;
    cam = cameraCreateDefinition();

    cameraSetLocationCoords(cam, 0.0, 1.0, 1.0);
    cameraSetTargetCoords(cam, 0.0, 0.0, 0.0);

    /* Test the reversibility of projection */
    Vector3 point = cameraProject(cam, v3(12.0, 5.2, -6.3));
    point = cameraUnproject(cam, point);
    ck_assert_vector_values(point, 12.0, 5.2, -6.3);

    cameraDeleteDefinition(cam);
}
END_TEST

START_TEST(test_camera_depth)
{
    CameraDefinition* cam;
    cam = cameraCreateDefinition();

    cameraSetLocationCoords(cam, 0.0, 0.0, 0.0);
    cameraSetTargetCoords(cam, 1.0, 0.0, 0.0);

    /* Test the real depth getter */
    Vector3 point = cameraProject(cam, v3(12.5, 0.0, 0.0));
    ck_assert_double_eq(cameraGetRealDepth(cam, point), 12.5);

    cameraDeleteDefinition(cam);
}
END_TEST

TEST_CASE(camera,
        test_camera_definition,
        test_camera_projection,
        test_camera_depth)

