#include "System.h"

#include <QCoreApplication>
#include <QThread>

static int core_count = -1;

int System::getCoreCount() {
    if (core_count < 0) {
#ifdef NDEBUG
        core_count = QThread::idealThreadCount();
        if (core_count <= 0) {
            qWarning("Can't find number of CPU cores, using only 1");
            core_count = 1;
        }
#else
        core_count = 1;
#endif
    }
    return core_count;
}

int System::getProcessId() {
    return QCoreApplication::applicationPid();
}
