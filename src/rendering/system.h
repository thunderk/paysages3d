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

typedef GMutex Mutex;

static inline Mutex* mutexCreate()
{
#ifdef GLIB_VERSION_2_32
    Mutex* mutex = malloc(sizeof(Mutex));
    g_mutex_init(mutex);
    return mutex;
#else
    return g_mutex_new();
#endif
}

static inline void mutexDestroy(Mutex* mutex)
{
#ifdef GLIB_VERSION_2_32
    g_mutex_clear(mutex);
    free(mutex);
#else
    g_mutex_free(mutex);
#endif
}

static inline void mutexAcquire(Mutex* mutex)
{
    g_mutex_lock(mutex);
}

static inline void mutexRelease(Mutex* mutex)
{
    g_mutex_unlock(mutex);
}

static inline void timeSleepMs(unsigned long ms)
{
    g_usleep(ms * 1000);
}
#endif

#ifdef __cplusplus
}
#endif

#endif
