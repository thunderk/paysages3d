#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <QtCore/QDebug>

using namespace CppUnit;

void noMessageOutput(QtMsgType, const QMessageLogContext&, const QString&)
{
}

int main()
{
    paysagesInit();

    qInstallMessageHandler(noMessageOutput);

    TextTestRunner runner;
    TestFactoryRegistry &registry = TestFactoryRegistry::getRegistry();

    runner.addTest(registry.makeTest());

    paysagesQuit();

    return runner.run() ? 1 : 0;
}

