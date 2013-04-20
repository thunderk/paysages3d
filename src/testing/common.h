#ifndef _PAYSAGES_TESTING_COMMON_H_
#define _PAYSAGES_TESTING_COMMON_H_

#include <check.h>
#include <stdarg.h>
#include <math.h>

static inline void _add_methods_to_case(TCase* tc, ...)
{
    void* method;
    va_list argp;

    va_start(argp, tc);
    while ((method = va_arg(argp, void*)) != NULL)
    {
        tcase_add_test(tc, method);
    }
    va_end(argp);
}

#define TEST_CASE(_name_,...) void test_ ## _name_ ## _case(Suite* s) { \
    TCase *tc = tcase_create(#_name_); \
    _add_methods_to_case(tc, __VA_ARGS__, NULL); \
    suite_add_tcase(s, tc); \
}

static inline int _double_equals(double x, double y)
{
    return fabs(x - y) < 0.00000000001;
}
static inline int _double_not_equals(double x, double y)
{
    return fabs(x - y) >= 0.00000000001;
}

#define _ck_assert_double(F, X, O, Y) ck_assert_msg(F(X, Y), "Assertion '"#X#O#Y"' failed: "#X"==%f, "#Y"==%f", X, Y)
#define ck_assert_double_eq(X, Y) _ck_assert_double(_double_equals, X, ==, Y)
#define ck_assert_double_ne(X, Y) _ck_assert_double(_double_not_equals, X, !=, Y)

#endif
