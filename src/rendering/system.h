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

#ifdef __cplusplus
}
#endif

#endif
