#include "AtmosphereColorPreviewRenderer.h"

#include "SoftwareRenderer.h"
#include "AtmosphereRenderer.h"
#include "AtmosphereResult.h"
#include "CameraDefinition.h"
#include "SurfaceMaterial.h"
#include "Scenery.h"
#include "BasePreview.h"

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

    e1 = v1.sub(v0);
    e2 = v2.sub(v0);

    h = d.crossProduct(e2);
    a = e1.dotProduct(h);

    if (a > -0.00001 && a < 0.00001)
    {
        return 0;
    }

    f = 1.0 / a;
    s = p.sub(v0);
    u = f * s.dotProduct(h);

    if (u < 0.0 || u > 1.0)
    {
        return 0;
    }

    q = s.crossProduct(e1);
    v = f * d.dotProduct(q);

    if (v < 0.0 || u + v > 1.0)
    {
        return 0;
    }

    t = f * e2.dotProduct(q);

    if (t > 0.00001)
    {
        *hit = p.add(d.scale(t));
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

AtmosphereColorPreviewRenderer::AtmosphereColorPreviewRenderer(AtmosphereDefinition* definition, double heading):
    definition(definition), heading(heading)
{
    getScenery()->getCamera()->setLocation(Vector3(0.0, 7.0, 0.0));
}

void AtmosphereColorPreviewRenderer::bindEvent(BasePreview* preview)
{
    preview->configHdrToneMapping(true);
    preview->configScaling(0.5, 5.0, 0.5, 2.5);
}
void AtmosphereColorPreviewRenderer::updateEvent()
{
    getScenery()->setAtmosphere(definition);
    prepare();
}

Color AtmosphereColorPreviewRenderer::getColor2D(double x, double y, double)
{
    Vector3 eye = {0.0, 7.0, 0.0};
    Vector3 direction = {x, -y, -1.0};
    Vector3 hit, normal;
    Matrix4 rotation;

    rotation = Matrix4::newRotateY(heading);

    if (_checkHit(eye, direction, &hit, &normal))
    {
        Color color;

        normal = rotation.transform(normal);
        hit = rotation.transform(hit);

        color = applyLightingToSurface(hit, normal, MOUNT_MATERIAL);
        return applyMediumTraversal(hit, color);
    }
    else
    {
        direction = rotation.transform(direction);

        return getAtmosphereRenderer()->getSkyColor(direction).final;
    }
}
