#ifndef _PAYSAGES_CAMERA_H_
#define _PAYSAGES_CAMERA_H_

#include "shared/types.h"
#include "renderer.h"
#include "pack.h"

#ifdef __cplusplus
extern "C" {
#endif

void cameraInit();
void cameraQuit();
void cameraSave(PackStream* stream, CameraDefinition* camera);
void cameraLoad(PackStream* stream, CameraDefinition* camera);

CameraDefinition cameraCreateDefinition();
void cameraDeleteDefinition(CameraDefinition* definition);
void cameraCopyDefinition(CameraDefinition* source, CameraDefinition* destination);
void cameraValidateDefinition(CameraDefinition* definition, int check_above);

void cameraSetLocation(CameraDefinition* camera, float x, float y, float z);
void cameraSetTarget(CameraDefinition* camera, float x, float y, float z);
void cameraSetRoll(CameraDefinition* camera, float angle);

void cameraStrafeForward(CameraDefinition* camera, float value);
void cameraStrafeRight(CameraDefinition* camera, float value);
void cameraStrafeUp(CameraDefinition* camera, float value);
void cameraRotateYaw(CameraDefinition* camera, float value);
void cameraRotatePitch(CameraDefinition* camera, float value);
void cameraRotateRoll(CameraDefinition* camera, float value);

void cameraSetRenderSize(CameraDefinition* camera, int width, int height);

Vector3 cameraProject(CameraDefinition* camera, Renderer* renderer, Vector3 point);
Vector3 cameraUnproject(CameraDefinition* camera, Renderer* renderer, Vector3 point);
void cameraProjectToFragment(CameraDefinition* camera, Renderer* renderer, float x, float y, float z, RenderFragment* result);
/*void cameraPushOverlay(CameraDefinition* camera, Color col, f_RenderFragmentCallback callback);*/
int cameraIsBoxInView(CameraDefinition* camera, Vector3 center, float xsize, float ysize, float zsize);
        
#ifdef __cplusplus
}
#endif

#endif
