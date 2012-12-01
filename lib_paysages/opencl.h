#ifndef _PAYSAGES_OPENCL_H_
#define _PAYSAGES_OPENCL_H_

/* OpenCL usage */

#ifdef __cplusplus
extern "C" {
#endif

#include "noise.h"

void openclInit();
int openclAvailable();
void openclQuit();

#ifdef __cplusplus
}
#endif

#endif
