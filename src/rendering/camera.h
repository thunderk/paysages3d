#ifndef _PAYSAGES_CAMERA_H_
#define _PAYSAGES_CAMERA_H_

#include "rendering_global.h"
#include "PackStream.h"
#include "tools/euclid.h"
#include "tools/boundingbox.h"

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

RENDERINGSHARED_EXPORT void cameraSave(PackStream* stream, CameraDefinition* camera);
RENDERINGSHARED_EXPORT void cameraLoad(PackStream* stream, CameraDefinition* camera);

RENDERINGSHARED_EXPORT CameraDefinition* cameraCreateDefinition();
RENDERINGSHARED_EXPORT void cameraDeleteDefinition(CameraDefinition* definition);
RENDERINGSHARED_EXPORT void cameraCopyDefinition(CameraDefinition* source, CameraDefinition* destination);
RENDERINGSHARED_EXPORT void cameraValidateDefinition(CameraDefinition* definition, int check_above);

RENDERINGSHARED_EXPORT Vector3 cameraGetLocation(CameraDefinition* camera);
RENDERINGSHARED_EXPORT Vector3 cameraGetTarget(CameraDefinition* camera);
RENDERINGSHARED_EXPORT Vector3 cameraGetUpVector(CameraDefinition* camera);
RENDERINGSHARED_EXPORT double cameraGetRoll(CameraDefinition* camera);
RENDERINGSHARED_EXPORT Vector3 cameraGetDirection(CameraDefinition* camera);
RENDERINGSHARED_EXPORT Vector3 cameraGetDirectionNormalized(CameraDefinition* camera);
RENDERINGSHARED_EXPORT VectorSpherical cameraGetDirectionSpherical(CameraDefinition* camera);
RENDERINGSHARED_EXPORT CameraPerspective cameraGetPerspective(CameraDefinition* camera);
RENDERINGSHARED_EXPORT double cameraGetRealDepth(CameraDefinition* camera, Vector3 projected);

RENDERINGSHARED_EXPORT void cameraSetLocation(CameraDefinition* camera, Vector3 location);
RENDERINGSHARED_EXPORT void cameraSetLocationCoords(CameraDefinition* camera, double x, double y, double z);
RENDERINGSHARED_EXPORT void cameraSetTarget(CameraDefinition* camera, Vector3 target);
RENDERINGSHARED_EXPORT void cameraSetTargetCoords(CameraDefinition* camera, double x, double y, double z);
RENDERINGSHARED_EXPORT void cameraSetRoll(CameraDefinition* camera, double angle);
RENDERINGSHARED_EXPORT void cameraSetZoomToTarget(CameraDefinition* camera, double zoom);

RENDERINGSHARED_EXPORT void cameraStrafeForward(CameraDefinition* camera, double value);
RENDERINGSHARED_EXPORT void cameraStrafeRight(CameraDefinition* camera, double value);
RENDERINGSHARED_EXPORT void cameraStrafeUp(CameraDefinition* camera, double value);
RENDERINGSHARED_EXPORT void cameraRotateYaw(CameraDefinition* camera, double value);
RENDERINGSHARED_EXPORT void cameraRotatePitch(CameraDefinition* camera, double value);
RENDERINGSHARED_EXPORT void cameraRotateRoll(CameraDefinition* camera, double value);

RENDERINGSHARED_EXPORT void cameraSetRenderSize(CameraDefinition* camera, int width, int height);

RENDERINGSHARED_EXPORT Vector3 cameraProject(CameraDefinition* camera, Vector3 point);
RENDERINGSHARED_EXPORT Vector3 cameraUnproject(CameraDefinition* camera, Vector3 point);
/*void cameraPushOverlay(CameraDefinition* camera, Color col, f_RenderFragmentCallback callback);*/
RENDERINGSHARED_EXPORT int cameraIsBoxInView(CameraDefinition* camera, Vector3 center, double xsize, double ysize, double zsize);
RENDERINGSHARED_EXPORT int cameraIsUnprojectedBoxInView(CameraDefinition* camera, BoundingBox* box);
RENDERINGSHARED_EXPORT int cameraIsProjectedBoxInView(CameraDefinition* camera, BoundingBox* box);

RENDERINGSHARED_EXPORT int cameraTransitionToAnother(CameraDefinition* current, CameraDefinition* wanted, double factor);

#ifdef __cplusplus
}
#endif

#endif
