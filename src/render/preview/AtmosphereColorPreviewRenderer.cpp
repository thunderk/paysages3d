#include "AtmosphereColorPreviewRenderer.h"

#include "SoftwareRenderer.h"
#include "AtmosphereRenderer.h"
#include "camera.h"
#include "tools/lighting.h"

/*
 * Atmosphere previews.
 */

typedef struct
{
    Vector3 location;
    double size;
} Mount;

#define MOUNTS_COUNT 11
static Mount MOUNTS[MOUNTS_COUNT] = {
    {
        {2.0, 0.0, -6.0}, 4.0
    },
    {
        {-1.5, 0.0, -8.0}, 4.0
    },
    {
        {3.0, 0.0, -10.0}, 6.0
    },
    {
        {-8.0, 0.0, -15.0}, 6.0
    },
    {
        {10.0, 0.0, -20.0}, 6.0
    },
    {
        {-6.0, 0.0, -30.0}, 8.0
    },
    {
        {15.0, 0.0, -40.0}, 8.0
    },
    {
        {-20.0, 0.0, -50.0}, 8.0
    },
    {
        {10.0, 0.0, -60.0}, 10.0
    },
    {
        {-5.0, 0.0, -80.0}, 10.0
    },
    {
        {30.0, 0.0, -100.0}, 10.0
    },
};

static SurfaceMaterial MOUNT_MATERIAL = {
    {0.4, 0.4, 0.4, 1.0}, 0.0, 0.0, 0.0, 0.0, {0.0, 0.0, 0.0, 0.0}
};

static inline int _rayIntersectsTriangle(Vector3 p, Vector3 d, Vector3 v0, Vector3 v1, Vector3 v2, Vector3* hit)
{
    Vector3 e1, e2, h, s, q;
    double a, f, u, v, t;

    e1 = v3Sub(v1, v0);
    e2 = v3Sub(v2, v0);

    h = v3Cross(d, e2);
    a = v3Dot(e1, h);

    if (a > -0.00001 && a < 0.00001)
    {
        return 0;
    }

    f = 1.0 / a;
    s = v3Sub(p, v0);
    u = f * v3Dot(s, h);

    if (u < 0.0 || u > 1.0)
    {
        return 0;
    }

    q = v3Cross(s, e1);
    v = f * v3Dot(d, q);

    if (v < 0.0 || u + v > 1.0)
    {
        return 0;
    }

    t = f * v3Dot(e2, q);

    if (t > 0.00001)
    {
        *hit = v3Add(p, v3Scale(d, t));
        return 1;
    }
    else
    {
        return 0;
    }
}

static inline int _checkHitMountain(Vector3 eye, Vector3 direction, Mount mount, Vector3* hit)
{
    Vector3 v0 = mount.location;
    Vector3 v1 = mount.location;
    Vector3 v2 = mount.location;
    v0.x -= mount.size;
    v1.x += mount.size;
    v2.y += mount.size;
    return _rayIntersectsTriangle(eye, direction, v0, v1, v2, hit);
}

static inline int _checkHitGround(Vector3 eye, Vector3 direction, Vector3* hit)
{
    if (direction.y > -0.0001)
    {
        return 0;
    }
    else
    {
        hit->x = eye.x - direction.x * eye.y / direction.y;
        hit->y = 0.0;
        hit->z = eye.z - direction.z * eye.y / direction.y;
        return 1;
    }
}

static inline int _checkHit(Vector3 eye, Vector3 direction, Vector3* hit, Vector3* normal)
{
    int i;
    for (i = 0; i < MOUNTS_COUNT; i++)
    {
        if (_checkHitMountain(eye, direction, MOUNTS[i], hit))
        {
            *normal = VECTOR_SOUTH;
            return 1;
        }
    }

    *normal = VECTOR_UP;
    return _checkHitGround(eye, direction, hit);
}

AtmosphereColorPreviewRenderer::AtmosphereColorPreviewRenderer():
    heading(0.0)
{
    cameraSetLocation(render_camera, Vector3(0.0, 7.0, 0.0));
}

Color AtmosphereColorPreviewRenderer::getColor2D(double x, double y, double)
{
    Vector3 eye = {0.0, 7.0, 0.0};
    Vector3 direction = {x, y, -1.0};
    Vector3 hit, normal;
    Matrix4 rotation;

    rotation = m4NewRotateY(heading);

    if (_checkHit(eye, direction, &hit, &normal))
    {
        Color color;

        normal = m4Transform(rotation, normal);
        hit = m4Transform(rotation, hit);

        color = this->applyLightingToSurface(this, hit, normal, &MOUNT_MATERIAL);
        return this->atmosphere->applyAerialPerspective(this, hit, color).final;
    }
    else
    {
        direction = m4Transform(rotation, direction);

        return this->atmosphere->getSkyColor(this, direction).final;
    }
}