#ifndef BRUNETON_TEST_H
#define BRUNETON_TEST_H

#include "BaseTestCase.h"

class Bruneton_Test: public BaseTestCase
{
public:
    CPPUNIT_TEST_SUITE(Bruneton_Test);
    CPPUNIT_TEST(testBrunetonAerialPerspective);
    CPPUNIT_TEST(testBrunetonAerialPerspective1);
    CPPUNIT_TEST_SUITE_END();

    void testBrunetonAerialPerspective();
    void testBrunetonAerialPerspective1();
};

#endif // BRUNETON_TEST_H
