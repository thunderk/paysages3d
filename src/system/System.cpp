#include "System.h"

#include <QThread>
#include <QDebug>

static int core_count = -1;

int System::getCoreCount()
{
    if (core_count < 0)
    {
        core_count = QThread::idealThreadCount();
        if (core_count <= 0)
        {
            qWarning() << "Can't find number of CPU cores, using only 1";
            core_count = 1;
        }
    }
    return core_count;
}
