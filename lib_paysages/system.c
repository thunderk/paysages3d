#include "system.h"
#include <unistd.h>

#ifdef WIN32
#include <windows.h>
#endif

static int _core_count = 1;

static int _getCoreCount()
{
    int core_count = 1;
#ifdef WIN32
    DWORD processAffinityMask;
    DWORD systemAffinityMask;

    if (GetProcessAffinityMask( GetCurrentProcess(),
                                &processAffinityMask,
                                &systemAffinityMask)){
        processAffinityMask = (processAffinityMask & 0x55555555)
            + (processAffinityMask >> 1 & 0x55555555);
        processAffinityMask = (processAffinityMask & 0x33333333)
            + (processAffinityMask >> 2 & 0x33333333);
        processAffinityMask = (processAffinityMask & 0x0f0f0f0f)
            + (processAffinityMask >> 4 & 0x0f0f0f0f);
        processAffinityMask = (processAffinityMask & 0x00ff00ff)
            + (processAffinityMask >> 8 & 0x00ff00ff);
        core_count          = (processAffinityMask & 0x0000ffff)
            + (processAffinityMask >>16 & 0x0000ffff);
    }
#endif
#ifdef _SC_NPROCESSORS_ONLN
    core_count = (int)sysconf(_SC_NPROCESSORS_ONLN);
#endif
#ifdef DEBUG_ONETHREAD
    core_count = 1;
#endif
    return core_count;
}

void systemInit()
{
    g_thread_init(NULL);
    _core_count = _getCoreCount();
}

int systemGetCoreCount()
{
    return _core_count;
}
