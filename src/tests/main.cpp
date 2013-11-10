#include "BaseTestCase.h"
#include <QtCore/QDebug>
#include "../rendering/main.h"

void noMessageOutput(QtMsgType, const QMessageLogContext&, const QString&)
{
}

int main(int argc, char **argv)
{
    int result;

    paysagesInit();

    qInstallMessageHandler(noMessageOutput);

    testing::InitGoogleTest(&argc, argv);
    result = RUN_ALL_TESTS();

    paysagesQuit();

    return result;
}

