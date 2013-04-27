#ifndef _PAYSAGES_CAMERA_H_
#define _PAYSAGES_CAMERA_H_

#include "tools/pack.h"
#include "tools/euclid.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct CameraDefinition CameraDefinition;

typedef struct
{
    double yfov;
    double xratio;
    double znear;
    double zfar;
} CameraPerspective;

void cameraSave(PackStream* stream, CameraDefinition* camera);
void cameraLoad(PackStream* stream, CameraDefinition* camera);

CameraDefinition* cameraCreateDefinition();
void cameraDeleteDefinition(CameraDefinition* definition);
void cameraCopyDefinition(CameraDefinition* source, CameraDefinition* destination);
void cameraValidateDefinition(CameraDefinition* definition, int check_above);

Vector3 cameraGetLocation(CameraDefinition* camera);
Vector3 cameraGetTarget(CameraDefinition* camera);
Vector3 cameraGetUpVector(CameraDefinition* camera);
double cameraGetRoll(CameraDefinition* camera);
Vector3 cameraGetDirection(CameraDefinition* camera);
Vector3 cameraGetDirectionNormalized(CameraDefinition* camera);
VectorSpherical cameraGetDirectionSpherical(CameraDefinition* camera);
CameraPerspective cameraGetPerspective(CameraDefinition* camera);

void cameraSetLocation(CameraDefinition* camera, Vector3 location);
void cameraSetLocationCoords(CameraDefinition* camera, double x, double y, double z);
void cameraSetTarget(CameraDefinition* camera, Vector3 target);
void cameraSetTargetCoords(CameraDefinition* camera, double x, double y, double z);
void cameraSetRoll(CameraDefinition* camera, double angle);
void cameraSetZoomToTarget(CameraDefinition* camera, double zoom);

void cameraStrafeForward(CameraDefinition* camera, double value);
void cameraStrafeRight(CameraDefinition* camera, double value);
void cameraStrafeUp(CameraDefinition* camera, double value);
void cameraRotateYaw(CameraDefinition* camera, double value);
void cameraRotatePitch(CameraDefinition* camera, double value);
void cameraRotateRoll(CameraDefinition* camera, double value);

void cameraSetRenderSize(CameraDefinition* camera, int width, int height);

Vector3 cameraProject(CameraDefinition* camera, Vector3 point);
Vector3 cameraUnproject(CameraDefinition* camera, Vector3 point);
/*void cameraPushOverlay(CameraDefinition* camera, Color col, f_RenderFragmentCallback callback);*/
int cameraIsBoxInView(CameraDefinition* camera, Vector3 center, double xsize, double ysize, double zsize);

int cameraTransitionToAnother(CameraDefinition* current, CameraDefinition* wanted, double factor);

#ifdef __cplusplus
}
#endif

#endif
