#ifndef _PAYSAGES_EXPLORING_MAIN_H_
#define _PAYSAGES_EXPLORING_MAIN_H_

#include "exploring_global.h"

#include "rendering/renderer.h"
#include "rendering/camera.h"

#ifdef __cplusplus
extern "C" {
#endif

EXPLORINGSHARED_EXPORT void exploringInit();
EXPLORINGSHARED_EXPORT void exploringSetViewPort(int width, int height, CameraDefinition* camera);
EXPLORINGSHARED_EXPORT void exploringRenderFrame(Renderer* renderer);

EXPLORINGSHARED_EXPORT void exploringStartStandAlone();
EXPLORINGSHARED_EXPORT void exploringInterruptStandAlone();

#ifdef __cplusplus
}
#endif

#endif
