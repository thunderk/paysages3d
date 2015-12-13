#ifndef BASETESTCASE_H
#define BASETESTCASE_H

#include "gtest/gtest.h"

#undef EXPECT_DOUBLE_EQ
#define EXPECT_DOUBLE_EQ(_expected_, _value_) EXPECT_NEAR(_expected_, _value_, 0.0000000001)
#undef ASSERT_DOUBLE_EQ
#define ASSERT_DOUBLE_EQ(_expected_, _value_) ASSERT_NEAR(_expected_, _value_, 0.0000000001)

#define EXPECT_DOUBLE_IN_RANGE(_value_, _min_, _max_)                                                                  \
    EXPECT_GT(_value_, _min_);                                                                                         \
    EXPECT_LT(_value_, _max_)
#define ASSERT_DOUBLE_IN_RANGE(_value_, _min_, _max_)                                                                  \
    ASSERT_GT(_value_, _min_);                                                                                         \
    ASSERT_LT(_value_, _max_)

#define EXPECT_COLOR_RGBA(_col_, _r_, _g_, _b_, _a_)                                                                   \
    EXPECT_DOUBLE_EQ(_col_.r, _r_);                                                                                    \
    EXPECT_DOUBLE_EQ(_col_.g, _g_);                                                                                    \
    EXPECT_DOUBLE_EQ(_col_.b, _b_);                                                                                    \
    EXPECT_DOUBLE_EQ(_col_.a, _a_)
#define ASSERT_COLOR_RGBA(_col_, _r_, _g_, _b_, _a_)                                                                   \
    EXPECT_DOUBLE_EQ(_col_.r, _r_);                                                                                    \
    EXPECT_DOUBLE_EQ(_col_.g, _g_);                                                                                    \
    EXPECT_DOUBLE_EQ(_col_.b, _b_);                                                                                    \
    EXPECT_DOUBLE_EQ(_col_.a, _a_)

#define EXPECT_VECTOR3_COORDS(_vec_, _x_, _y_, _z_)                                                                    \
    EXPECT_DOUBLE_EQ(_vec_.x, _x_);                                                                                    \
    EXPECT_DOUBLE_EQ(_vec_.y, _y_);                                                                                    \
    EXPECT_DOUBLE_EQ(_vec_.z, _z_)
#define ASSERT_VECTOR3_COORDS(_vec_, _x_, _y_, _z_)                                                                    \
    ASSERT_DOUBLE_EQ(_vec_.x, _x_);                                                                                    \
    ASSERT_DOUBLE_EQ(_vec_.y, _y_);                                                                                    \
    ASSERT_DOUBLE_EQ(_vec_.z, _z_)

#define EXPECT_GLVARIABLE_FLOAT(_expected_, _state_, _name_)                                                           \
    EXPECT_NEAR(_expected_, (_state_)->get(_name_)->getFloatValue(), 0.000001)
#define ASSERT_GLVARIABLE_FLOAT(_expected_, _state_, _name_)                                                           \
    ASSERT_NEAR(_expected_, (_state_)->get(_name_)->getFloatValue(), 0.000001)

class BaseTestCase : public ::testing::Test {};

#endif // BASETESTCASE_H
