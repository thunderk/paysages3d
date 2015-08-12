#include "BaseTestCase.h"

#include <QtCore/QDebug>
#include "Logs.h"

void noMessageOutput(QtMsgType, const QMessageLogContext&, const QString&)
{
}

int main(int argc, char **argv)
{
    int result;

    qInstallMessageHandler(noMessageOutput);
    Logs::disable();

    testing::InitGoogleTest(&argc, argv);
    result = RUN_ALL_TESTS();

    return result;
}

