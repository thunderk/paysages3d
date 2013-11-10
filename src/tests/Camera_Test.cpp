#include "BaseTestCase.h"

#include "camera.h"

TEST(Camera, Definition)
{
    CameraDefinition* cam;
    cam = cameraCreateDefinition();

    cameraSetLocationCoords(cam, 0.0, 1.0, 1.0);
    cameraSetTargetCoords(cam, 0.0, 0.0, 0.0);

    EXPECT_VECTOR3_COORDS(cameraGetLocation(cam), 0.0, 1.0, 1.0);
    EXPECT_VECTOR3_COORDS(cameraGetTarget(cam), 0.0, 0.0, 0.0);

    cameraDeleteDefinition(cam);
}

TEST(Camera, Projection)
{
    CameraDefinition* cam;
    cam = cameraCreateDefinition();

    cameraSetLocationCoords(cam, 0.0, 1.0, 1.0);
    cameraSetTargetCoords(cam, 0.0, 0.0, 0.0);

    /* Test the reversibility of projection */
    Vector3 point = cameraProject(cam, v3(12.0, 5.2, -6.3));
    point = cameraUnproject(cam, point);
    EXPECT_VECTOR3_COORDS(point, 12.0, 5.2, -6.3);
    point = cameraProject(cam, v3(-25.1, 8.3, 1.3));
    point = cameraUnproject(cam, point);
    EXPECT_VECTOR3_COORDS(point, -25.1, 8.3, 1.3);

    cameraDeleteDefinition(cam);
}

TEST(Camera, Depth)
{
    CameraDefinition* cam;
    cam = cameraCreateDefinition();

    cameraSetLocationCoords(cam, 0.0, 0.0, 0.0);
    cameraSetTargetCoords(cam, 1.0, 0.0, 0.0);

    /* Test the real depth getter */
    Vector3 point = cameraProject(cam, v3(12.5, 0.0, 0.0));
    ASSERT_DOUBLE_EQ(cameraGetRealDepth(cam, point), 12.5);
    point = cameraProject(cam, v3(12.5, 8.0, -3.0));
    ASSERT_DOUBLE_EQ(cameraGetRealDepth(cam, point), 12.5);
    point = cameraProject(cam, v3(12.5, 12.0, 58.0));
    ASSERT_DOUBLE_EQ(cameraGetRealDepth(cam, point), 12.5);

    cameraDeleteDefinition(cam);
}
