#include "camera.h"

#include <stdlib.h>
#include <math.h>

#include "render.h"
#include "shared/types.h"
#include "shared/globals.h"
#include "shared/constants.h"
#include "shared/functions.h"
#include "scenery.h"

void cameraInit()
{
}

void cameraSave(FILE* f, CameraDefinition* camera)
{
    v3Save(camera->location, f);
    toolsSaveDouble(f, camera->yaw);
    toolsSaveDouble(f, camera->pitch);
    toolsSaveDouble(f, camera->roll);
}

void cameraLoad(FILE* f, CameraDefinition* camera)
{
    camera->location = v3Load(f);
    camera->yaw = toolsLoadDouble(f);
    camera->pitch = toolsLoadDouble(f);
    camera->roll = toolsLoadDouble(f);

    cameraValidateDefinition(camera, 0);
}

CameraDefinition cameraCreateDefinition()
{
    CameraDefinition definition;

    definition.location.x = 0.0;
    definition.location.y = 0.0;
    definition.location.z = 0.0;
    definition.yaw = 0.0;
    definition.pitch = 0.0;
    definition.roll = 0.0;

    cameraValidateDefinition(&definition, 0);

    return definition;
}

void cameraDeleteDefinition(CameraDefinition* definition)
{
}

void cameraCopyDefinition(CameraDefinition* source, CameraDefinition* destination)
{
    *destination = *source;
    
    cameraValidateDefinition(destination, 0);
}

void cameraValidateDefinition(CameraDefinition* definition, int check_above)
{
    WaterDefinition water;
    TerrainDefinition terrain;
    double water_height, terrain_height, diff;
    Vector3 move;
    Matrix4 rotation;
    
    if (check_above)
    {
        water = waterCreateDefinition();
        sceneryGetWater(&water);
        water_height = water.height + 0.5;
        waterDeleteDefinition(&water);

        terrain = terrainCreateDefinition();
        sceneryGetTerrain(&terrain);
        terrain_height = terrainGetHeight(&terrain, definition->location.x, definition->location.z) + 0.5;
        terrainDeleteDefinition(&terrain);
        
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
    
    definition->forward.x = 1.0;
    definition->forward.y = 0.0;
    definition->forward.z = 0.0;
    definition->right.x = 0.0;
    definition->right.y = 0.0;
    definition->right.z = 1.0;
    definition->up.x = 0.0;
    definition->up.y = 1.0;
    definition->up.z = 0.0;
    
    rotation = m4NewRotateEuler(definition->yaw, definition->pitch, definition->roll);
    
    definition->forward = m4MultPoint(rotation, definition->forward);
    definition->right = m4MultPoint(rotation, definition->right);
    definition->up = m4MultPoint(rotation, definition->up);
    
    definition->target = v3Add(definition->location, definition->forward);

    definition->project = m4Mult(m4NewPerspective(1.57, 1.333333, 1.0, 1000.0), m4NewLookAt(definition->location, definition->target, definition->up));
    definition->unproject = m4Inverse(definition->project);
}

void cameraSetLocation(CameraDefinition* camera, double x, double y, double z)
{
    camera->location.x = x;
    camera->location.y = y;
    camera->location.z = z;

    cameraValidateDefinition(camera, 0);
}

void cameraSetTarget(CameraDefinition* camera, double x, double y, double z)
{
    Vector3 forward, target;
    
    target.x = x;
    target.y = y;
    target.z = z;
    
    forward = v3Sub(target, camera->location);
    if (v3Norm(forward) < 0.0000001)
    {
        return;
    }
    forward = v3Normalize(forward);
    
    if (fabs(forward.x) < 0.0000001 && fabs(forward.z) < 0.0000001)
    {
        /* Forward vector is vertical */
        if (forward.y > 0.0)
        {
            camera->pitch = M_PI_2;
        }
        else if (forward.y > 0.0)
        {
            camera->pitch = -M_PI_2;
        }
    }
    else
    {
        /* TODO Guess angles */
    }

    cameraValidateDefinition(camera, 0);
}

void cameraSetRoll(CameraDefinition* camera, double angle)
{
    camera->roll = angle;

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
    camera->yaw += value;

    cameraValidateDefinition(camera, 0);
}

void cameraRotatePitch(CameraDefinition* camera, double value)
{
    camera->pitch += value;

    cameraValidateDefinition(camera, 0);
}

void cameraRotateRoll(CameraDefinition* camera, double value)
{
    camera->roll += value;

    cameraValidateDefinition(camera, 0);
}

Vector3 cameraProject(CameraDefinition* camera, Vector3 point)
{
    point = m4Transform(camera->project, point);
    point.x = (point.x + 1.0) * 0.5 * (double)render_width;
    point.y = (-point.y + 1.0) * 0.5 * (double)render_height;
    return point;
}

Vector3 cameraUnproject(CameraDefinition* camera, Vector3 point)
{
    point.x = (point.x / (0.5 * (double)render_width) - 1.0);
    point.y = -(point.y / (0.5 * (double)render_height) - 1.0);
    return m4Transform(camera->unproject, point);
}

void cameraProjectToFragment(CameraDefinition* camera, double x, double y, double z, RenderFragment* result)
{
    Vector3 point = {x, y, z};
    point = cameraProject(camera, point);
    result->x = lround(point.x);
    result->y = lround(point.y);
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
    /*Vertex v1, v2, v3, v4;
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

    renderPushQuad(&v1, &v2, &v3, &v4);*/
}
