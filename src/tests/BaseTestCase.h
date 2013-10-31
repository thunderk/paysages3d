#ifndef BASETESTCASE_H
#define BASETESTCASE_H

#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>
using namespace CppUnit;

class BaseTestCase: public TestFixture
{
public:
    BaseTestCase();
};

#endif // BASETESTCASE_H
