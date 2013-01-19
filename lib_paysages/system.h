#ifndef _PAYSAGES_SYSTEM_H_
#define _PAYSAGES_SYSTEM_H_

/* Library dependent features */

#include "tools/color.h"
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* (*ThreadFunction)(void* data);
typedef Color (*PictureCallbackSavePixel)(void* data, int x, int y);
typedef void (*PictureCallbackLoadStarted)(void* data, int width, int height);
typedef void (*PictureCallbackLoadPixel)(void* data, int x, int y, Color col);

void systemInit();
int systemGetCoreCount();

int systemSavePictureFile(const char* filepath, PictureCallbackSavePixel callback_pixel, void* data, int width, int height);
int systemLoadPictureFile(const char* filepath, PictureCallbackLoadStarted callback_start, PictureCallbackLoadPixel callback_pixel, void* data);

int systemGetFileSize(const char* path);

#ifdef HAVE_GLIB
#include <glib.h>

typedef GThread Thread;

static inline Thread* threadCreate(ThreadFunction function, void* data)
{
#ifdef GLIB_VERSION_2_32
    return g_thread_new("thread", (GThreadFunc)function, data);
#else
    GError* error;
    return g_thread_create((GThreadFunc)function, data, 1, &error);
#endif
}

static inline void* threadJoin(Thread* thread)
{
    return g_thread_join(thread);
}

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
