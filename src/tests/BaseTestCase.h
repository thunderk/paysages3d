#ifndef BASETESTCASE_H
#define BASETESTCASE_H

#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>
using namespace CppUnit;

// Transitional macros from C libcheck
static inline int _double_equals(double x, double y)
{
    return fabs(x - y) < 0.00000000001;
}
static inline int _double_not_equals(double x, double y)
{
    return fabs(x - y) >= 0.00000000001;
}
static inline int _double_greater(double x, double y)
{
    return _double_not_equals(x, y) && (x > y);
}
static inline int _double_greater_or_equal(double x, double y)
{
    return _double_equals(x, y) || (x >= y);
}
static inline int _double_less(double x, double y)
{
    return _double_not_equals(x, y) && (x < y);
}
static inline int _double_less_or_equal(double x, double y)
{
    return _double_equals(x, y) || (x <= y);
}

#define _ck_assert_double(F, X, O, Y) CPPUNIT_ASSERT(F(X, Y), "Assertion '"#X#O#Y"' failed: "#X"=%.12f, "#Y"=%.12f", X, Y)
#define ck_assert_double_eq(X, Y) _ck_assert_double(_double_equals, X, ==, Y)
#define ck_assert_double_ne(X, Y) _ck_assert_double(_double_not_equals, X, !=, Y)
#define ck_assert_double_gt(X, Y) _ck_assert_double(_double_greater, X, >, Y)
#define ck_assert_double_lt(X, Y) _ck_assert_double(_double_less, X, <, Y)
#define ck_assert_double_gte(X, Y) _ck_assert_double(_double_greater_or_equal, X, >=, Y)
#define ck_assert_double_lte(X, Y) _ck_assert_double(_double_less_or_equal, X, <=, Y)

class BaseTestCase: public TestFixture
{
public:
    BaseTestCase();
};

#endif // BASETESTCASE_H
