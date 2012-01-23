#include "camera.h"

#include <stdlib.h>
#include <math.h>

#include "shared/types.h"
#include "shared/globals.h"
#include "shared/constants.h"
#include "shared/functions.h"

void cameraInit()
{
}

void cameraSave(FILE* f, CameraDefinition* camera)
{
    v3Save(camera->location, f);
    v3Save(camera->target, f);
}

void cameraLoad(FILE* f, CameraDefinition* camera)
{
    camera->location = v3Load(f);
    camera->target = v3Load(f);

    cameraValidateDefinition(camera);
}

CameraDefinition cameraCreateDefinition()
{
    CameraDefinition definition;

    definition.location.x = -1.0;
    definition.location.y = 0.0;
    definition.location.z = 0.0;
    definition.target.x = 0.0;
    definition.target.y = 0.0;
    definition.target.z = 0.0;

    cameraValidateDefinition(&definition);

    return definition;
}

void cameraDeleteDefinition(CameraDefinition* definition)
{
}

void cameraCopyDefinition(CameraDefinition* source, CameraDefinition* destination)
{
    *destination = *source;
}

void cameraValidateDefinition(CameraDefinition* definition)
{
    /* TODO Recompute up vector */
    definition->up.x = 0.0;
    definition->up.y = 1.0;
    definition->up.z = 0.0;

    definition->project = m4Mult(m4NewPerspective(1.57, 1.333333, 1.0, 1000.0), m4NewLookAt(VECTOR_ZERO, v3Sub(definition->target, definition->location), definition->up));
    definition->unproject = m4Inverse(definition->project);
}

void cameraSetLocation(CameraDefinition* camera, double x, double y, double z)
{
    camera->location.x = x;
    camera->location.y = y;
    camera->location.z = z;

    cameraValidateDefinition(camera);
}

void cameraSetTarget(CameraDefinition* camera, double x, double y, double z)
{
    camera->target.x = x;
    camera->target.y = y;
    camera->target.z = z;

    cameraValidateDefinition(camera);
}

void cameraSetAngle(CameraDefinition* camera, double angle)
{
    /* TODO */
}

Vector3 cameraProject(CameraDefinition* camera, Vector3 point)
{
    point = m4Transform(camera->project, v3Sub(point, camera->location));
    point.x = (-point.x + 1.0) * 0.5 * (double)render_width;
    point.y = (-point.y + 1.0) * 0.5 * (double)render_height;
    return point;
}

Vector3 cameraUnproject(CameraDefinition* camera, Vector3 point)
{
    point.x = -(point.x / (0.5 * (double)render_width) - 1.0);
    point.y = -(point.y / (0.5 * (double)render_height) - 1.0);
    return v3Add(m4Transform(camera->unproject, point), camera->location);
}

void cameraProjectToFragment(CameraDefinition* camera, double x, double y, double z, RenderFragment* result)
{
    Vector3 point = {x, y, z};
    point = m4Transform(camera->project, v3Sub(point, camera->location));
    result->x = lround((-point.x + 1.0) * 0.5 * (double)render_width);
    result->y = lround((-point.y + 1.0) * 0.5 * (double)render_height);
    result->z = point.z;
}

/**
 * Render a quad that will fill the view in front of the camera.
 * This quad can be used for post-processing.
 *
 * @param col Color of the polygon.
 * @param callback Post-processing callback.
 */
void cameraPushOverlay(CameraDefinition* camera, Color col, f_RenderFragmentCallback callback)
{
    Vertex v1, v2, v3, v4;
    Vector3 v;

    v.x = 0.0;
    v.y = 0.0;
    v.z = 10.0;
    v1.location = cameraUnproject(camera, v);
    v1.color = col;
    v1.callback = callback;

    v.x = 0.0;
    v.y = (double)render_height;
    v.z = 10.0;
    v2.location = cameraUnproject(camera, v);
    v2.color = col;
    v2.callback = callback;

    v.x = (double)render_width;
    v.y = (double)render_height;
    v.z = 10.0;
    v3.location = cameraUnproject(camera, v);
    v3.color = col;
    v3.callback = callback;

    v.x = (double)render_width;
    v.y = 0.0;
    v.z = 10.0;
    v4.location = cameraUnproject(camera, v);
    v4.color = col;
    v4.callback = callback;

    renderPushQuad(&v1, &v2, &v3, &v4);
}
