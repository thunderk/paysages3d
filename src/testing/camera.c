#include "testing/common.h"

#include "rendering/camera.h"

#define _checkVector(_vector_, _x_, _y_, _z_) ck_assert_double_eq(_vector_.x, _x_);ck_assert_double_eq(_vector_.y, _y_);ck_assert_double_eq(_vector_.z, _z_)

START_TEST(test_camera_definition)
{
    CameraDefinition* cam;
    cam = cameraCreateDefinition();

    cameraSetLocationCoords(cam, 0.0, 1.0, 1.0);
    cameraSetTargetCoords(cam, 0.0, 0.0, 0.0);

    _checkVector(cameraGetLocation(cam), 0.0, 1.0, 1.0);
    _checkVector(cameraGetTarget(cam), 0.0, 1.0, 1.0);

    cameraDeleteDefinition(cam);
}
END_TEST

TEST_CASE(camera, test_camera_definition)

