#include "VegetationModelRenderer.h"

#include "Color.h"
#include "Disk.h"
#include "InfiniteRay.h"
#include "LightComponent.h"
#include "SoftwareRenderer.h"
#include "SpaceSegment.h"
#include "SurfaceMaterial.h"
#include "VegetationModelDefinition.h"
#include "VegetationResult.h"

#ifndef NDEBUG
//#define DEBUG_VEGETATION_CONTAINERS 1
#endif
#ifdef DEBUG_VEGETATION_CONTAINERS
SurfaceMaterial DEBUG_MATERIAL1(Color(1.0, 0.0, 0.0));
#endif

VegetationModelRenderer::VegetationModelRenderer(SoftwareRenderer *parent, const VegetationModelDefinition *model)
    : parent(parent), model(model) {
}

VegetationModelRenderer::~VegetationModelRenderer() {
}

VegetationResult VegetationModelRenderer::getResult(const SpaceSegment &segment, bool only_hit) const {
    InfiniteRay ray(segment.getStart(), segment.getDirection());
    int intersections;
    const SurfaceMaterial *material = &SurfaceMaterial::getDefault();
    bool hit = false;
    Vector3 location, normal;
    double distance, nearest, maximal;

    maximal = segment.getLength();
    nearest = maximal;

    for (const auto &branch : model->getSolidVolumes()) {
        Vector3 near, far;
        if (branch.findRayIntersection(ray, &near, &far)) {
            distance = ray.getCursor(near);
            if (distance >= 0.0 and distance <= maximal) {
                // Got a branch hit
                if (only_hit) {
                    return VegetationResult(true);
                }

                if (distance < nearest) {
                    material = &model->getSolidMaterial();
                    nearest = distance;

                    hit = true;
                    location = near;
                    normal = near.sub(branch.getAxis().getOrigin())
                                 .crossProduct(branch.getAxis().getDirection())
                                 .normalize();
                    normal = branch.getAxis().getDirection().crossProduct(normal).normalize();
                }
            }
        }
    }

    for (const auto &foliage : model->getFoliageGroups()) {
        intersections = foliage.checkRayIntersection(ray);
        if (intersections == 2) {
            InfiniteRay subray(ray.getOrigin().sub(foliage.getCenter()).scale(1.0 / foliage.getRadius()),
                               ray.getDirection());

            for (const auto &leaf : model->getFoliageItems()) {
                Sphere leafcap(leaf.getPoint(), leaf.getRadius() * leaf.getRadius() / foliage.getRadius());
                // TODO Add cap intersection to Sphere class
                Vector3 near, far;
                if (leafcap.findRayIntersection(subray, &near, &far) == 2) {
                    if (near.sub(leaf.getPoint()).normalize().dotProduct(leaf.getNormal()) < 0.5) {
                        if (far.sub(leaf.getPoint()).normalize().dotProduct(leaf.getNormal()) < 0.5) {
                            continue;
                        } else {
                            near = far;
                        }
                    }

                    Vector3 capnormal = near.sub(leaf.getPoint()).normalize();
                    near = near.scale(foliage.getRadius()).add(foliage.getCenter());
                    distance = ray.getCursor(near);

                    if (distance >= 0.0 and distance <= maximal) {
                        // Got a foliage hit
                        if (only_hit) {
                            return VegetationResult(true);
                        }

                        if (distance < nearest) {
                            material = &model->getFoliageMaterial();
                            nearest = distance;

                            hit = true;
                            location = near;
                            normal = capnormal;

                            if (normal.dotProduct(location.sub(segment.getStart())) > 0.0) {
                                // We look at backside
                                normal = normal.scale(-1.0);
                            }
                        }
                    }
                }
            }
#ifdef DEBUG_VEGETATION_CONTAINERS
            if (!hit) {
                Vector3 near, far;
                intersections = foliage.findRayIntersection(ray, &near, &far);
                location = near;
                normal = VECTOR_ZERO;
                material = &DEBUG_MATERIAL1;
                hit = true;
            }
#endif
        }
    }

    if (hit) {
        return VegetationResult(location, normal, *material);
    } else {
        return VegetationResult();
    }
}

bool VegetationModelRenderer::applyLightFilter(LightComponent &light, const Vector3 &at) {
    return getResult(SpaceSegment(at, light.direction.scale(-2.0)), true).isHit();
}
