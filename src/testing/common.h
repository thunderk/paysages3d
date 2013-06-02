#ifndef _PAYSAGES_TESTING_COMMON_H_
#define _PAYSAGES_TESTING_COMMON_H_

#include <stdlib.h>
#include <stdio.h>
#include <check.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>

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

/***** Boolean assertions *****/
#define ck_assert_true(_X_) ck_assert_int_ne((_X_), 0)
#define ck_assert_false(_X_) ck_assert_int_eq((_X_), 0)

/***** Floating point assertions *****/
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

#define _ck_assert_double(F, X, O, Y) ck_assert_msg(F(X, Y), "Assertion '"#X#O#Y"' failed: "#X"=%f, "#Y"=%f", X, Y)
#define ck_assert_double_eq(X, Y) _ck_assert_double(_double_equals, X, ==, Y)
#define ck_assert_double_ne(X, Y) _ck_assert_double(_double_not_equals, X, !=, Y)
#define ck_assert_double_gt(X, Y) _ck_assert_double(_double_greater, X, >, Y)
#define ck_assert_double_lt(X, Y) _ck_assert_double(_double_less, X, <, Y)
#define ck_assert_double_gte(X, Y) _ck_assert_double(_double_greater_or_equal, X, >=, Y)
#define ck_assert_double_lte(X, Y) _ck_assert_double(_double_less_or_equal, X, <=, Y)


/***** Generic comparison assertions *****/
#define DEFINE_COMPARE_ASSERT(_type_, _cmpfunc_, _strfunc_) \
static inline int _ck_gen_##_type_##_cmp(_type_ X, _type_ Y) { \
    return _cmpfunc_(X, Y); \
} \
static inline char* _ck_gen_##_type_##_str(char* buffer, _type_ X) { \
    _strfunc_(X, buffer, 99); \
    buffer[100] = '\0'; \
    return buffer; \
}
#define ck_assert_generic_eq(_type_, X, Y) ck_assert_msg(_ck_gen_##_type_##_cmp(X, Y) == 0, "Assertion '"#X"=="#Y"' failed : "#X"=%s, "#Y"=%s", _ck_gen_##_type_##_str(_ck_gen_strbuf1, X), _ck_gen_##_type_##_str(_ck_gen_strbuf2, Y))

static char _ck_gen_strbuf1[101];
static char _ck_gen_strbuf2[101];


/***** Some builtin comparisons *****/
#define ck_assert_double_in_range(_double_, _x_, _y_) ck_assert_double_gte(_double_, _x_);ck_assert_double_lte(_double_, _y_)
#define ck_assert_vector_values(_vector_, _x_, _y_, _z_) ck_assert_double_eq(_vector_.x, _x_);ck_assert_double_eq(_vector_.y, _y_);ck_assert_double_eq(_vector_.z, _z_)

#endif
