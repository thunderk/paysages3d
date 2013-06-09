#include <check.h>
#include <stdlib.h>

#include "rendering/main.h"

extern void test_euclid_case(Suite* s);
extern void test_camera_case(Suite* s);
extern void test_clouds_case(Suite* s);
extern void test_noise_case(Suite* s);

int main(int argc, char** argv)
{
    int number_failed;
    Suite *s = suite_create("rendering");

    paysagesInit();

    /* TODO Find a way to automate this */
    test_euclid_case(s);
    test_camera_case(s);
    test_clouds_case(s);
    test_noise_case(s);

    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    paysagesQuit();

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
