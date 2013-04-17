#ifndef _PAYSAGES_EXPLORING_MAIN_H_
#define _PAYSAGES_EXPLORING_MAIN_H_

#include "rendering/renderer.h"
#include "rendering/camera.h"

#ifdef __cplusplus
extern "C" {
#endif

void exploringInit();
void exploringSetViewPort(int width, int height, CameraDefinition* camera);
void exploringRenderFrame(Renderer* renderer);

void exploringStartStandAlone();
void exploringInterruptStandAlone();

#ifdef __cplusplus
}
#endif

#endif
