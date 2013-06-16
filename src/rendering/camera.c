#include "camera.h"

#include <stdlib.h>
#include <math.h>
#include "render.h"
#include "scenery.h"
#include "tools.h"

struct CameraDefinition
{
    /* Definition */
    Vector3 location;
    VectorSpherical direction;
    double roll;

    /* Projection info */
    double width;
    double height;
    CameraPerspective perspective;

    /* Auto updated */
    Vector3 target;
    Vector3 forward;
    Vector3 right;
    Vector3 up;
    Matrix4 project;
    Matrix4 unproject;
};

void cameraSave(PackStream* stream, CameraDefinition* camera)
{
    v3Save(stream, &camera->location);
    packWriteDouble(stream , &camera->direction.r);
    packWriteDouble(stream, &camera->direction.phi);
    packWriteDouble(stream, &camera->direction.theta);
    packWriteDouble(stream, &camera->roll);
}

void cameraLoad(PackStream* stream, CameraDefinition* camera)
{
    v3Load(stream, &camera->location);
    packReadDouble(stream, &camera->direction.r);
    packReadDouble(stream, &camera->direction.phi);
    packReadDouble(stream, &camera->direction.theta);
    packReadDouble(stream, &camera->roll);

    cameraValidateDefinition(camera, 0);
}

CameraDefinition* cameraCreateDefinition()
{
    CameraDefinition* definition;

    definition = malloc(sizeof (CameraDefinition));

    definition->location.x = 0.0;
    definition->location.y = 0.0;
    definition->location.z = 0.0;
    definition->direction.phi = 0.0;
    definition->direction.theta = 0.0;
    definition->direction.r = 1.0;
    definition->roll = 0.0;

    definition->width = 1.0;
    definition->height = 1.0;
    definition->perspective.yfov = 1.57;
    definition->perspective.xratio = 1.0;
    definition->perspective.znear = 1.0;
    definition->perspective.zfar = 1000.0;

    cameraValidateDefinition(definition, 0);

    return definition;
}

void cameraDeleteDefinition(CameraDefinition* definition)
{
    free(definition);
}

void cameraCopyDefinition(CameraDefinition* source, CameraDefinition* destination)
{
    *destination = *source;

    cameraValidateDefinition(destination, 0);
}

void cameraValidateDefinition(CameraDefinition* definition, int check_above)
{
    Renderer* renderer;
    double water_height, terrain_height, diff;
    Vector3 move;
    Matrix4 rotation;

    if (check_above)
    {
        renderer = sceneryCreateStandardRenderer();
        terrain_height = renderer->terrain->getHeight(renderer, definition->location.x, definition->location.z, 1) + 0.5;
        water_height = renderer->water->getHeightInfo(renderer).max_height + 0.5;
        rendererDelete(renderer);

        if (definition->location.y < water_height || definition->location.y < terrain_height)
        {
            if (water_height > terrain_height)
            {
                diff = water_height - definition->location.y;
            }
            else
            {
                diff = terrain_height - definition->location.y;
            }

            move.x = move.z = 0.0;
            move.y = diff;
            definition->location = v3Add(definition->location, move);
        }
    }

    if (definition->location.y > 300.0)
    {
        definition->location.y = 300.0;
    }

    definition->forward.x = 1.0;
    definition->forward.y = 0.0;
    definition->forward.z = 0.0;
    definition->right.x = 0.0;
    definition->right.y = 0.0;
    definition->right.z = 1.0;
    definition->up.x = 0.0;
    definition->up.y = 1.0;
    definition->up.z = 0.0;

    rotation = m4NewRotateEuler(definition->direction.phi, definition->direction.theta, definition->roll);

    definition->forward = m4MultPoint(rotation, definition->forward);
    definition->right = m4MultPoint(rotation, definition->right);
    definition->up = m4MultPoint(rotation, definition->up);

    definition->target = v3Add(definition->location, v3FromSpherical(definition->direction));

    definition->project = m4Mult(m4NewPerspective(definition->perspective.yfov, definition->perspective.xratio, definition->perspective.znear, definition->perspective.zfar), m4NewLookAt(definition->location, definition->target, definition->up));
    definition->unproject = m4Inverse(definition->project);
}

Vector3 cameraGetLocation(CameraDefinition* camera)
{
    return camera->location;
}

Vector3 cameraGetTarget(CameraDefinition* camera)
{
    return camera->target;
}

Vector3 cameraGetUpVector(CameraDefinition* camera)
{
    return camera->up;
}

double cameraGetRoll(CameraDefinition* camera)
{
    return camera->roll;
}

Vector3 cameraGetDirection(CameraDefinition* camera)
{
    return v3FromSpherical(camera->direction);
}

Vector3 cameraGetDirectionNormalized(CameraDefinition* camera)
{
    return camera->forward;
}

VectorSpherical cameraGetDirectionSpherical(CameraDefinition* camera)
{
    return camera->direction;
}

CameraPerspective cameraGetPerspective(CameraDefinition* camera)
{
    return camera->perspective;
}

double cameraGetRealDepth(CameraDefinition* camera, Vector3 projected)
{
    /* TODO Optimize this */
    Matrix4 m = m4NewPerspective(camera->perspective.yfov, camera->perspective.xratio, camera->perspective.znear, camera->perspective.zfar);
    projected.x = (projected.x / (0.5 * camera->width) - 1.0);
    projected.y = -(projected.y / (0.5 * camera->height) - 1.0);
    return m4Transform(m4Inverse(m), projected).z;
}

void cameraSetLocation(CameraDefinition* camera, Vector3 location)
{
    camera->location = location;

    cameraValidateDefinition(camera, 0);
}

void cameraSetLocationCoords(CameraDefinition* camera, double x, double y, double z)
{
    Vector3 v = {x, y, z};
    cameraSetLocation(camera, v);
}

void cameraSetTarget(CameraDefinition* camera, Vector3 target)
{
    Vector3 forward;

    forward = v3Sub(target, camera->location);
    if (v3Norm(forward) < 0.0000001)
    {
        return;
    }

    camera->direction = v3ToSpherical(forward);

    cameraValidateDefinition(camera, 0);
}

void cameraSetTargetCoords(CameraDefinition* camera, double x, double y, double z)
{
    Vector3 v = {x, y, z};
    cameraSetTarget(camera, v);
}

void cameraSetRoll(CameraDefinition* camera, double angle)
{
    camera->roll = angle;

    cameraValidateDefinition(camera, 0);
}

void cameraSetZoomToTarget(CameraDefinition* camera, double zoom)
{
    camera->direction.r = zoom;
    camera->location = v3Add(camera->target, v3Scale(v3FromSpherical(camera->direction), -1.0));

    cameraValidateDefinition(camera, 0);
}

void cameraStrafeForward(CameraDefinition* camera, double value)
{
    camera->location = v3Add(camera->location, v3Scale(camera->forward, value));

    cameraValidateDefinition(camera, 0);
}

void cameraStrafeRight(CameraDefinition* camera, double value)
{
    camera->location = v3Add(camera->location, v3Scale(camera->right, value));

    cameraValidateDefinition(camera, 0);
}

void cameraStrafeUp(CameraDefinition* camera, double value)
{
    camera->location = v3Add(camera->location, v3Scale(camera->up, value));

    cameraValidateDefinition(camera, 0);
}

void cameraRotateYaw(CameraDefinition* camera, double value)
{
    camera->direction.phi += value;

    cameraValidateDefinition(camera, 0);
}

void cameraRotatePitch(CameraDefinition* camera, double value)
{
    camera->direction.theta += value;

    cameraValidateDefinition(camera, 0);
}

void cameraRotateRoll(CameraDefinition* camera, double value)
{
    camera->roll += value;

    cameraValidateDefinition(camera, 0);
}

void cameraSetRenderSize(CameraDefinition* camera, int width, int height)
{
    camera->width = (double)width;
    camera->height = (double)height;
    camera->perspective.xratio = camera->width / camera->height;

    cameraValidateDefinition(camera, 0);
}

Vector3 cameraProject(CameraDefinition* camera, Vector3 point)
{
    point = m4Transform(camera->project, point);
    if (point.z < 1.0)
    {
        point.x = -point.x;
        point.y = -point.y;
    }
    point.x = (point.x + 1.0) * 0.5 * camera->width;
    point.y = (-point.y + 1.0) * 0.5 * camera->height;
    return point;
}

Vector3 cameraUnproject(CameraDefinition* camera, Vector3 point)
{
    point.x = (point.x / (0.5 * camera->width) - 1.0);
    point.y = -(point.y / (0.5 * camera->height) - 1.0);
    if (point.z < 1.0)
    {
        point.x = -point.x;
        point.y = -point.y;
    }
    return m4Transform(camera->unproject, point);
}

/**
 * Render a quad that will fill the view in front of the camera.
 * This quad can be used for post-processing.
 *
 * @param col Color of the polygon.
 * @param callback Post-processing callback.
 */

/*void cameraPushOverlay(CameraDefinition* camera, Color col, f_RenderFragmentCallback callback)
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
}*/

static inline void _updateBox(Vector3* point, double* xmin, double* xmax, double* ymin, double* ymax, double* zmax)
{
    *xmin = (*xmin < point->x) ? *xmin : point->x;
    *ymin = (*ymin < point->y) ? *ymin : point->y;

    *xmax = (*xmax > point->x) ? *xmax : point->x;
    *ymax = (*ymax > point->y) ? *ymax : point->y;
    *zmax = (*zmax > point->z) ? *zmax : point->z;
}

int cameraIsBoxInView(CameraDefinition* camera, Vector3 center, double xsize, double ysize, double zsize)
{
    Vector3 projected;
    double xmin, xmax, ymin, ymax, zmax;

    center.x -= xsize / 2.0;
    center.y -= ysize / 2.0;
    center.z -= zsize / 2.0;
    projected = cameraProject(camera, center);
    xmin = xmax = projected.x;
    ymin = ymax = projected.y;
    zmax = projected.z;

    center.x += xsize;
    projected = cameraProject(camera, center);
    _updateBox(&projected, &xmin, &xmax, &ymin, &ymax, &zmax);

    center.z += zsize;
    projected = cameraProject(camera, center);
    _updateBox(&projected, &xmin, &xmax, &ymin, &ymax, &zmax);

    center.x -= xsize;
    projected = cameraProject(camera, center);
    _updateBox(&projected, &xmin, &xmax, &ymin, &ymax, &zmax);

    center.y += ysize;
    projected = cameraProject(camera, center);
    _updateBox(&projected, &xmin, &xmax, &ymin, &ymax, &zmax);

    center.x += xsize;
    projected = cameraProject(camera, center);
    _updateBox(&projected, &xmin, &xmax, &ymin, &ymax, &zmax);

    center.z -= zsize;
    projected = cameraProject(camera, center);
    _updateBox(&projected, &xmin, &xmax, &ymin, &ymax, &zmax);

    center.x -= xsize;
    projected = cameraProject(camera, center);
    _updateBox(&projected, &xmin, &xmax, &ymin, &ymax, &zmax);

    return xmin <= camera->width && xmax >= 0.0 && ymin <= camera->height && ymax >= 0.0 && zmax >= camera->perspective.znear;
}

int cameraTransitionToAnother(CameraDefinition* current, CameraDefinition* wanted, double factor)
{
    double dx, dy, dz, dr, dphi, dtheta, droll;

    dx = wanted->location.x - current->location.x;
    dy = wanted->location.y - current->location.y;
    dz = wanted->location.z - current->location.z;
    dr = wanted->direction.r - current->direction.r;
    dphi = wanted->direction.phi - current->direction.phi;
    dtheta = wanted->direction.theta - current->direction.theta;
    droll = wanted->roll - current->roll;

    if (fabs(dx) < 0.000001 && fabs(dy) < 0.000001 && fabs(dz) < 0.000001 && fabs(dr) < 0.000001 && fabs(dphi) < 0.000001 && fabs(dtheta) < 0.000001 && fabs(droll) < 0.000001)
    {
        return 0;
    }
    else
    {
        current->location.x += dx * factor;
        current->location.y += dy * factor;
        current->location.z += dz * factor;
        current->direction.r += dr * factor;
        current->direction.phi += dphi * factor;
        current->direction.theta += dtheta * factor;
        current->roll += droll * factor;

        cameraValidateDefinition(current, 0);
        return 1;
    }
}
