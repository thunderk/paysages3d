#ifndef _PAYSAGES_OPENCL_H_
#define _PAYSAGES_OPENCL_H_

/* OpenCL usage */

#include "rendering_global.h"

RENDERINGSHARED_EXPORT void openclInit();
RENDERINGSHARED_EXPORT int openclAvailable();
RENDERINGSHARED_EXPORT void openclQuit();

#endif
