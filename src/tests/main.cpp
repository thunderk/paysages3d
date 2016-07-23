#include "BaseTestCase.h"

#include "Logs.h"
#include <QtCore/QDebug>

void noMessageOutput(QtMsgType, const QMessageLogContext &, const QString &) {
}

int main(int argc, char **argv) {
    int result;

    qInstallMessageHandler(noMessageOutput);
#ifdef NDEBUG
    Logs::disable();
#endif

    testing::InitGoogleTest(&argc, argv);
    result = RUN_ALL_TESTS();

    return result;
}
