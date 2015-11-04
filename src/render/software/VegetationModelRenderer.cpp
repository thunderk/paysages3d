#include "VegetationModelRenderer.h"

#include "Color.h"
#include "SurfaceMaterial.h"
#include "SpaceSegment.h"
#include "InfiniteRay.h"
#include "Disk.h"
#include "SoftwareRenderer.h"
#include "LightComponent.h"
#include "VegetationModelDefinition.h"
#include "VegetationResult.h"

VegetationModelRenderer::VegetationModelRenderer(SoftwareRenderer* parent, const VegetationModelDefinition* model):
    parent(parent), model(model)
{
}

VegetationModelRenderer::~VegetationModelRenderer()
{
}

VegetationResult VegetationModelRenderer::getResult(const SpaceSegment &segment, bool only_hit) const
{
    InfiniteRay ray(segment.getStart(), segment.getDirection());
    int intersections;
    Color result = COLOR_TRANSPARENT;
    bool hit = false;
    Vector3 location, normal;
    double distance, nearest, maximal;

    maximal = segment.getLength();
    nearest = maximal;

    for (const auto &branch: model->getSolidVolumes())
    {
        Vector3 near, far;
        if (branch.checkRayIntersection(ray, &near, &far))
        {
            distance = ray.getCursor(near);
            if (distance >= 0.0 and distance <= maximal)
            {
                // Got a branch hit
                if (only_hit)
                {
                    return VegetationResult(true);
                }

                if (distance < nearest)
                {
                    result = Color(0.2, 0.15, 0.15);
                    nearest = distance;

                    hit = true;
                    location = near;
                    normal = near.sub(branch.getAxis().getOrigin()).crossProduct(branch.getAxis().getDirection()).normalize();
                    normal = branch.getAxis().getDirection().crossProduct(normal).normalize();
                }
            }
        }
    }

    for (const auto &foliage: model->getFoliageGroups())
    {
        Vector3 near, far;
        intersections = foliage.checkRayIntersection(ray, &near, &far);
        if (intersections == 2)
        {
            InfiniteRay subray(ray.getOrigin().sub(foliage.getCenter()).scale(1.0 / foliage.getRadius()), ray.getDirection());

            for (const auto &leaf: model->getFoliageItems())
            {
                Sphere leafcap(leaf.getPoint(), leaf.getRadius() * leaf.getRadius() / foliage.getRadius());
                // TODO Add cap intersection to Sphere class
                Vector3 capnear, capfar;
                if (leafcap.checkRayIntersection(subray, &capnear, &capfar) == 2)
                {
                    if (capnear.sub(leaf.getPoint()).normalize().dotProduct(leaf.getNormal()) < 0.5)
                    {
                        if (capfar.sub(leaf.getPoint()).normalize().dotProduct(leaf.getNormal()) < 0.5)
                        {
                            continue;
                        }
                        else
                        {
                            capnear = capfar;
                        }
                    }

                    Vector3 capnormal = capnear.sub(leaf.getPoint()).normalize();
                    capnear = capnear.scale(foliage.getRadius()).add(foliage.getCenter());
                    distance = ray.getCursor(capnear);

                    if (distance >= 0.0 and distance <= maximal)
                    {
                        // Got a foliage hit
                        if (only_hit)
                        {
                            return VegetationResult(true);
                        }

                        if (distance < nearest)
                        {
                            result = Color(0.3, 0.5, 0.3);
                            nearest = distance;

                            hit = true;
                            location = capnear;
                            normal = capnormal;

                            if (normal.dotProduct(location.sub(segment.getStart())) > 0.0)
                            {
                                // We look at backside
                                result = Color(0.3, 0.4, 0.3);
                                normal = normal.scale(-1.0);
                            }
                        }

                    }
                }
            }
        }
    }

    if (hit)
    {
        SurfaceMaterial material(result);
        material.reflection = 0.001;
        material.shininess = 2.0;
        material.hardness = 0.1;
        material.validate();
        return VegetationResult(location, normal, material);
    }
    else
    {
        return VegetationResult();
    }
}

bool VegetationModelRenderer::applyLightFilter(LightComponent &light, const Vector3 &at)
{
    return getResult(SpaceSegment(at, light.direction.scale(-2.0)), true).isHit();
}
