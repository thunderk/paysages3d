#include "CloudBasicLayerRenderer.h"

#include "AtmosphereRenderer.h"
#include "AtmosphereResult.h"
#include "CloudLayerDefinition.h"
#include "Curve.h"
#include "FloatNode.h"
#include "LightComponent.h"
#include "Logs.h"
#include "Maths.h"
#include "NoiseGenerator.h"
#include "SoftwareRenderer.h"
#include "SurfaceMaterial.h"
#include "clouds/BaseCloudsModel.h"
#include <algorithm>
#include <cassert>
#include <cmath>

struct CloudSegment {
    Vector3 start;
    Vector3 end;
    double length;
};

CloudBasicLayerRenderer::CloudBasicLayerRenderer(SoftwareRenderer *parent) : BaseCloudLayerRenderer(parent) {
}

/**
 * Go through the cloud layer to find segments (parts of the lookup that are inside the cloud).
 *
 * definition - The cloud layer
 * renderer - The renderer environment
 * start - Start position of the lookup (already optimized)
 * direction - Normalized direction of the lookup
 * detail - Level of noise detail required
 * max_segments - Maximum number of segments to collect
 * max_inside_length - Maximum length to spend inside the cloud
 * max_total_length - Maximum lookup length
 * inside_length - Resulting length inside cloud (sum of all segments length)
 * total_length - Resulting lookup length
 * out_segments - Allocated space to fill found segments
 *
 * Returns the number of segments found.
 */
int CloudBasicLayerRenderer::findSegments(BaseCloudsModel *model, const Vector3 &start, const Vector3 &direction,
                                          int max_segments, double max_inside_length, double max_total_length,
                                          double *inside_length, double *total_length, CloudSegment *out_segments,
                                          double base_detail) {
    double ymin, ymax;
    int inside, segment_count;
    double current_total_length, current_inside_length;
    double step_length, segment_length;
    double min_step, max_step;
    double noise_distance, previous_noise_distance;
    Vector3 walker, step, segment_start, offset;
    double render_precision;

    if (max_segments <= 0) {
        return 0;
    }

    model->getAltitudeRange(&ymin, &ymax);

    model->getDetailRange(&min_step, &max_step);

    double distance = parent->getCameraLocation().sub(start).getNorm();
    render_precision = min_step + (max_step - min_step) * min(distance / (quality + 0.1), 100.0) * 0.01;

    /*double verticality = fabs(direction.y);
    if (verticality > 0.5) {
        render_precision *= 1.0 - 1.8 * (verticality - 0.5);
    }*/

    segment_count = 0;
    current_total_length = 0.0;
    current_inside_length = 0.0;
    segment_length = 0.0;
    walker = start;
    offset = Vector3(model->getLayer()->propXOffset()->getValue(), 0.0, model->getLayer()->propZOffset()->getValue());
    noise_distance = model->getDensity(start.add(offset), base_detail);
    previous_noise_distance = noise_distance;
    inside = noise_distance > 0.0;
    if (inside) {
        segment_start = start;
    }
    step = direction.scale(render_precision);
    bool stop = false;

    do {
        walker = walker.add(step);
        step_length = step.getNorm();
        noise_distance = stop ? 0.0 : model->getDensity(walker.add(offset), base_detail);
        current_total_length += step_length;

        if (noise_distance > 0.0) {
            if (inside) {
                // inside the cloud
                segment_length += step_length;
                current_inside_length += step_length;
                step = direction.scale((noise_distance < 1.0) ? render_precision : (noise_distance * render_precision));
            } else {
                // entering the cloud
                inside = 1;
                segment_length = step_length - Maths::zeroPoint(step_length, previous_noise_distance, noise_distance);
                assert(segment_length >= 0.0 && segment_length <= step_length);
                segment_start = walker.add(direction.scale(-segment_length));
                current_inside_length += segment_length;
                step = direction.scale(render_precision);
            }
        } else {
            if (inside) {
                // exiting the cloud
                double exit_length = Maths::zeroPoint(step_length, previous_noise_distance, noise_distance);
                assert(exit_length >= 0.0);
                segment_length += exit_length;
                current_inside_length += exit_length;

                out_segments->start = segment_start;
                out_segments->end = walker.add(direction.scale(exit_length - step_length));
                out_segments->length = segment_length;
                out_segments++;
                if (++segment_count >= max_segments) {
                    break;
                }

                inside = 0;
                step = direction.scale(render_precision);
            } else {
                // searching for a cloud
                step =
                    direction.scale((noise_distance > -1.0) ? render_precision : (-noise_distance * render_precision));
            }
        }

        render_precision *= 1.0 + 0.001 / (quality + 0.1);
        previous_noise_distance = noise_distance;
        stop = not(walker.y >= ymin - 0.001 && walker.y <= ymax + 0.001 && current_total_length < max_total_length &&
                   current_inside_length < max_inside_length);
    } while (inside or not stop);

    *total_length = current_total_length;
    *inside_length = current_inside_length;
    return segment_count;
}

static inline Vector3 _getPseudoNormal(const BaseCloudsModel *model, const Vector3 &base, const Vector3 &direction) {
    double precision = 0.3;
    double base_density = model->getDensity(base, precision); // TODO keep
    double density = model->getDensity(base.add(direction.scale(precision * 10.0)), precision);
    double diff = base_density - density;
    return direction.scale(diff > 0.0 ? diff : 0.0);
}

Color CloudBasicLayerRenderer::getColor(BaseCloudsModel *model, const Vector3 &eye, const Vector3 &location) {
    int i, segment_count;
    double max_length, total_length, inside_length;
    Vector3 start, end, direction;
    Color result, col;
    CloudSegment segments[30];

    start = eye;
    end = location;
    if (!optimizeSearchLimits(model, &start, &end)) {
        return COLOR_TRANSPARENT;
    }

    direction = end.sub(start);
    max_length = direction.getNorm();
    direction = direction.normalize();
    result = COLOR_TRANSPARENT;

    double ymin, ymax;
    model->getAltitudeRange(&ymin, &ymax);
    double transparency_depth = (ymax - ymin);

    SurfaceMaterial material(COLOR_WHITE.scaled(8.0));
    material.ambient = 0.8;
    material.hardness = 0.0;
    material.reflection = 0.2;
    material.shininess = 3.0;
    material.validate();

    segment_count = findSegments(model, start, direction, 30, transparency_depth, max_length, &inside_length,
                                 &total_length, segments, 0.001);
    for (i = segment_count - 1; i >= 0; i--) {
        Vector3 normal = VECTOR_ZERO;
        const Vector3 &location = segments[i].start;
        normal = normal.add(_getPseudoNormal(model, location, VECTOR_UP));
        normal = normal.add(_getPseudoNormal(model, location, VECTOR_DOWN));
        normal = normal.add(_getPseudoNormal(model, location, VECTOR_EAST));
        normal = normal.add(_getPseudoNormal(model, location, VECTOR_WEST));
        normal = normal.add(_getPseudoNormal(model, location, VECTOR_NORTH));
        normal = normal.add(_getPseudoNormal(model, location, VECTOR_SOUTH));
        col = parent->applyLightingToSurface(location, normal.normalize(), material);

        col.a = (segments[i].length >= transparency_depth) ? 1.0 : (segments[i].length / transparency_depth);
        result.mask(col);
    }

    // Opacity
    result.a = Maths::smoothstep(0.0, transparency_depth, inside_length);

    // Apply aerial perspective
    if (result.a > 0.00001) {
        assert(segment_count > 0);

        double a = result.a;
        // TODO Don't apply it only at first segment
        result = parent->getAtmosphereRenderer()->applyAerialPerspective(segments[0].start, result).final;
        result.a = a;
    }

    return result;
}

bool CloudBasicLayerRenderer::alterLight(BaseCloudsModel *model, LightComponent *light, const Vector3 &,
                                         const Vector3 &location) {
    Vector3 start, end, direction;
    double inside_depth, total_depth, factor;
    CloudSegment segments[30];

    start = location;
    direction = light->direction.scale(-1.0);
    end = location.add(direction.scale(10000.0));
    if (not optimizeSearchLimits(model, &start, &end)) {
        return true;
    }

    double ymin, ymax;
    model->getAltitudeRange(&ymin, &ymax);
    double light_traversal = (ymax - ymin) * 0.8 * light->color.getPower();
    findSegments(model, start, direction, 30, light_traversal, end.sub(start).getNorm(), &inside_depth, &total_depth,
                 segments, 0.1);

    if (light_traversal < 0.0001) {
        factor = 0.0;
    } else {
        factor = inside_depth / light_traversal;
        if (factor > 1.0) {
            factor = 1.0;
        } else if (factor > 0.00001) {
            factor = sqrt(factor);
        }
    }

    double miminum_light = 0.01 * light->color.getPower();
    factor = 1.0 - (1.0 - miminum_light) * factor;

    light->color.r *= factor;
    light->color.g *= factor;
    light->color.b *= factor;

    return true;
}
