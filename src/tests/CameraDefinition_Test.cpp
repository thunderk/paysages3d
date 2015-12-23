#include "BaseTestCase.h"

#include "CameraDefinition.h"

TEST(CameraDefinition, constructor) {
    CameraDefinition cam;

    cam.setLocationCoords(0.0, 1.0, 1.0);
    cam.setTargetCoords(0.0, 0.0, 0.0);

    EXPECT_VECTOR3_COORDS(cam.getLocation(), 0.0, 1.0, 1.0);
    EXPECT_VECTOR3_COORDS(cam.getTarget(), 0.0, 0.0, 0.0);
}

TEST(CameraDefinition, unproject) {
    CameraDefinition cam;
    Vector3 point;

    cam.setLocationCoords(0.0, 1.0, 1.0);
    cam.setTargetCoords(0.0, 0.0, 0.0);

    // Test the reversibility of projection
    point = cam.project(Vector3(12.0, 5.2, -6.3));
    point = cam.unproject(point);
    EXPECT_VECTOR3_COORDS(point, 12.0, 5.2, -6.3);
    point = cam.project(Vector3(-25.1, 8.3, 1.3));
    point = cam.unproject(point);
    EXPECT_VECTOR3_COORDS(point, -25.1, 8.3, 1.3);
}

TEST(CameraDefinition, getRealDepth) {
    CameraDefinition cam;

    cam.setLocationCoords(0.0, 0.0, 0.0);
    cam.setTargetCoords(1.0, 0.0, 0.0);

    // Test the real depth getter
    Vector3 point = cam.project(Vector3(12.5, 0.0, 0.0));
    ASSERT_DOUBLE_EQ(cam.getRealDepth(point), 12.5);
    point = cam.project(Vector3(12.5, 8.0, -3.0));
    ASSERT_DOUBLE_EQ(cam.getRealDepth(point), 12.5);
    point = cam.project(Vector3(12.5, 12.0, 58.0));
    ASSERT_DOUBLE_EQ(cam.getRealDepth(point), 12.5);
}
