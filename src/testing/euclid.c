#include "testing/common.h"
#include "rendering/tools/euclid.h"

static inline int _Vector3_cmp(Vector3 v1, Vector3 v2)
{
    return fabs(v1.x - v2.z) >= 0.00000000001 && fabs(v1.y - v2.y) >= 0.00000000001 && fabs(v1.z - v2.z) >= 0.00000000001;
}

static inline void _Vector3_str(Vector3 v, char* buffer, int length)
{
    snprintf(buffer, length, "(%f,%f,%f)", v.x, v.y, v.z);
}
DEFINE_COMPARE_ASSERT(Vector3, _Vector3_cmp, _Vector3_str);

START_TEST(test_euclid_angles)
{
    ck_assert_double_eq(euclidGet2DAngle(0.0, 0.0), 0.0);

    ck_assert_double_eq(euclidGet2DAngle(0.1, 0.0), 0.0);
    ck_assert_double_eq(euclidGet2DAngle(1.0, 0.0), 0.0);
    ck_assert_double_eq(euclidGet2DAngle(2.0, 0.0), 0.0);

    ck_assert_double_eq(euclidGet2DAngle(0.0, 0.1), M_PI_2);
    ck_assert_double_eq(euclidGet2DAngle(0.0, 1.0), M_PI_2);
    ck_assert_double_eq(euclidGet2DAngle(0.0, 2.0), M_PI_2);

    ck_assert_double_eq(euclidGet2DAngle(-0.1, 0.0), M_PI);
    ck_assert_double_eq(euclidGet2DAngle(-1.0, 0.0), M_PI);
    ck_assert_double_eq(euclidGet2DAngle(-2.0, 0.0), M_PI);

    ck_assert_double_eq(euclidGet2DAngle(0.0, -0.1), 3.0 * M_PI_2);
    ck_assert_double_eq(euclidGet2DAngle(0.0, -1.0), 3.0 * M_PI_2);
    ck_assert_double_eq(euclidGet2DAngle(0.0, -2.0), 3.0 * M_PI_2);

    ck_assert_double_eq(euclidGet2DAngle(0.5, 0.5), M_PI_4);
    ck_assert_double_eq(euclidGet2DAngle(0.5, -0.5), 7.0 * M_PI_4);
    ck_assert_double_eq(euclidGet2DAngle(-0.5, 0.5), 3.0 * M_PI_4);
    ck_assert_double_eq(euclidGet2DAngle(-0.5, -0.5), 5.0 * M_PI_4);
}

END_TEST

START_TEST(test_vectors)
{
    Vector3 v1, v2, v3;

    /* Test scaling */
    v1.x = 0.0;
    v1.y = -4.3;
    v1.z = 8.2;
    v2.x = 0.0;
    v2.y = -8.6;
    v2.z = 16.4;
    v3.x = 0.0;
    v3.y = 2.15;
    v3.z = -4.1;
    ck_assert_generic_eq(Vector3, v3Scale(v1, 2.0), v2);
    ck_assert_generic_eq(Vector3, v3Scale(v1, -0.5), v3);
}

END_TEST

START_TEST(test_vectors_spherical)
{
    Vector3 v1;
    VectorSpherical v2;

    /* Test conversion to spherical */
    v1.x = v1.y = v1.z = 0.5;
    v2 = v3ToSpherical(v1);
    ck_assert_double_eq(v2.r, sqrt(0.5 * 0.5 + 0.5 * 0.5 + 0.5 * 0.5));
    ck_assert_double_eq(v2.phi, M_PI_4);
    ck_assert_double_eq(v2.theta, M_PI_2 - 0.955316618125);
}
END_TEST

TEST_CASE(euclid, test_euclid_angles, test_vectors_spherical)

