#ifndef _PAYSAGES_SYSTEM_H_
#define _PAYSAGES_SYSTEM_H_

/* Library dependent features */

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void systemInit();
int systemGetCoreCount();

int systemGetFileSize(const char* path);

#ifdef HAVE_GLIB
#include <glib.h>

static inline void timeSleepMs(unsigned long ms)
{
    g_usleep(ms * 1000);
}
#endif

#ifdef __cplusplus
}
#endif

#endif
