#ifndef _PAYSAGES_OPENCL_H_
#define _PAYSAGES_OPENCL_H_

/* OpenCL usage */

#include "rendering_global.h"
#include "noise.h"

#ifdef __cplusplus
extern "C" {
#endif

RENDERINGSHARED_EXPORT void openclInit();
RENDERINGSHARED_EXPORT int openclAvailable();
RENDERINGSHARED_EXPORT void openclQuit();

#ifdef __cplusplus
}
#endif

#endif
