#include "system.h"

#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

#ifndef NDEBUG
#define DEBUG_ONETHREAD 1
#endif

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
#ifndef GLIB_VERSION_2_32
    g_thread_init(NULL);
#endif
    _core_count = _getCoreCount();
}

int systemGetCoreCount()
{
    return _core_count;
}

int systemGetFileSize(const char* path)
{
    struct stat st;
    stat(path, &st);
    return st.st_size;
}
