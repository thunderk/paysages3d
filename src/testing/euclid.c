#include "testing/common.h"
#include "rendering/tools/euclid.h"

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

TEST_CASE(euclid, test_euclid_angles)
