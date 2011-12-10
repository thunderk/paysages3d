#include <stdlib.h>
#include <math.h>

#include "shared/types.h"
#include "shared/globals.h"
#include "shared/constants.h"
#include "shared/functions.h"

Vector3 camera_location;
static Vector3 camera_target;
static Vector3 camera_up;
static Matrix4 matrix_project;
static Matrix4 matrix_project_inverse;

static void __updateMatrix()
{
    /* TODO Recompute up vector */
    camera_up.x = 0.0;
    camera_up.y = 1.0;
    camera_up.z = 0.0;

    matrix_project = m4Mult(m4NewPerspective(1.57, 1.333333, 1.0, 1000.0), m4NewLookAt(VECTOR_ZERO, v3Sub(camera_target, camera_location), camera_up));
    matrix_project_inverse = m4Inverse(matrix_project);
}

void cameraSave(FILE* f)
{
}

void cameraLoad(FILE* f)
{
}

void cameraSetLocation(double x, double y, double z)
{
    camera_location.x = x;
    camera_location.y = y;
    camera_location.z = z;
    __updateMatrix();
}

void cameraSetTarget(double x, double y, double z)
{
    camera_target.x = x;
    camera_target.y = y;
    camera_target.z = z;
    __updateMatrix();
}

void cameraSetAngle(double angle)
{
    /* TODO */
}

Vector3 cameraProject(Vector3 point)
{
    point = m4Transform(matrix_project, v3Sub(point, camera_location));
    point.x = (-point.x + 1.0) * 0.5 * (double)render_width;
    point.y = (-point.y + 1.0) * 0.5 * (double)render_height;
    return point;
}

Vector3 cameraUnproject(Vector3 point)
{
    point.x = -(point.x / (0.5 * (double)render_width) - 1.0);
    point.y = -(point.y / (0.5 * (double)render_height) - 1.0);
    return v3Add(m4Transform(matrix_project_inverse, point), camera_location);
}

void cameraProjectToFragment(double x, double y, double z, RenderFragment* result)
{
    Vector3 point = {x, y, z};
    point = m4Transform(matrix_project, v3Sub(point, camera_location));
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
void cameraPushOverlay(Color col, f_RenderFragmentCallback callback)
{
    Vertex v1, v2, v3, v4;
    Vector3 v;
    
    v.x = 0.0;
    v.y = 0.0;
    v.z = 10.0;
    v1.location = cameraUnproject(v);
    v1.color = col;
    v1.callback = callback;

    v.x = 0.0;
    v.y = (double)render_height;
    v.z = 10.0;
    v2.location = cameraUnproject(v);
    v2.color = col;
    v2.callback = callback;
   
    v.x = (double)render_width;
    v.y = (double)render_height;
    v.z = 10.0;
    v3.location = cameraUnproject(v);
    v3.color = col;
    v3.callback = callback;

    v.x = (double)render_width;
    v.y = 0.0;
    v.z = 10.0;
    v4.location = cameraUnproject(v);
    v4.color = col;
    v4.callback = callback;
    
    renderPushQuad(&v1, &v2, &v3, &v4);
}
