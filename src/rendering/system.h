#ifndef _PAYSAGES_SYSTEM_H_
#define _PAYSAGES_SYSTEM_H_

/* Library dependent features */

#include "rendering_global.h"
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

RENDERINGSHARED_EXPORT void systemInit();
RENDERINGSHARED_EXPORT int systemGetCoreCount();

RENDERINGSHARED_EXPORT int systemGetFileSize(const char* path);

#ifdef __cplusplus
}
#endif

#endif
