#include "CloudBasicLayerRenderer.h"

#include <cassert>
#include <cmath>
#include <algorithm>
#include "CloudLayerDefinition.h"
#include "SoftwareRenderer.h"
#include "NoiseGenerator.h"
#include "Curve.h"
#include "AtmosphereRenderer.h"
#include "AtmosphereResult.h"
#include "LightComponent.h"
#include "clouds/BaseCloudsModel.h"
#include "SurfaceMaterial.h"
#include "Logs.h"
#include "FloatNode.h"

struct CloudSegment {
    Vector3 start;
    Vector3 end;
    double length;
};

CloudBasicLayerRenderer::CloudBasicLayerRenderer(SoftwareRenderer *parent) : BaseCloudLayerRenderer(parent) {
}

static inline double _getDistanceToBorder(BaseCloudsModel *model, const Vector3 &position) {
    return model->getDensity(position);
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
                                          double *inside_length, double *total_length, CloudSegment *out_segments) {
    double ymin, ymax;
    int inside, segment_count;
    double current_total_length, current_inside_length;
    double step_length, segment_length;
    double min_step, max_step;
    double noise_distance;
    Vector3 walker, step, segment_start, offset;
    double render_precision;

    if (max_segments <= 0) {
        return 0;
    }

    model->getAltitudeRange(&ymin, &ymax);

    model->getDetailRange(&min_step, &max_step);

    double distance = parent->getCameraLocation().sub(start).getNorm();
    render_precision = min_step + (max_step - min_step) * min(distance / (quality + 0.1), 100.0) * 0.01;

    segment_count = 0;
    current_total_length = 0.0;
    current_inside_length = 0.0;
    segment_length = 0.0;
    walker = start;
    offset = Vector3(model->getLayer()->propXOffset()->getValue(), 0.0, model->getLayer()->propZOffset()->getValue());
    noise_distance = _getDistanceToBorder(model, start.add(offset)) * render_precision;
    inside = 0;
    step = direction.scale(render_precision);

    do {
        walker = walker.add(step);
        step_length = step.getNorm();
        noise_distance = _getDistanceToBorder(model, walker.add(offset)) * render_precision;
        current_total_length += step_length;

        if (noise_distance > 0.0) {
            if (inside) {
                // inside the cloud
                segment_length += step_length;
                current_inside_length += step_length;
                step = direction.scale((noise_distance < render_precision) ? render_precision : noise_distance);
            } else {
                // entering the cloud
                inside = 1;
                segment_length = 0.0;
                segment_start = walker;
                current_inside_length += segment_length;
                step = direction.scale(render_precision);
            }
        } else {
            if (inside) {
                // exiting the cloud
                segment_length += step_length;
                current_inside_length += step_length;

                out_segments->start = segment_start;
                out_segments->end = walker;
                out_segments->length = segment_length;
                out_segments++;
                if (++segment_count >= max_segments) {
                    break;
                }

                inside = 0;
                step = direction.scale(render_precision);
            } else {
                // searching for a cloud
                step = direction.scale((noise_distance > -render_precision) ? render_precision : -noise_distance);
            }
        }

        render_precision *= 1.0 + 0.001 / (quality + 0.1);
    } while (inside || (walker.y >= ymin - 0.001 && walker.y <= ymax + 0.001 &&
                        current_total_length < max_total_length && current_inside_length < max_inside_length));

    *total_length = current_total_length;
    *inside_length = current_inside_length;
    return segment_count;
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

    SurfaceMaterial material(COLOR_WHITE.scaled(5.0));
    material.hardness = 1.0;
    material.reflection = 0.0;
    material.shininess = 0.0;
    material.validate();

    segment_count = findSegments(model, start, direction, 30, transparency_depth, max_length, &inside_length,
                                 &total_length, segments);
    for (i = segment_count - 1; i >= 0; i--) {
        col = parent->applyLightingToSurface(segments[i].start, VECTOR_UP, material);

        col.a = (segments[i].length >= transparency_depth) ? 1.0 : (segments[i].length / transparency_depth);
        result.mask(col);
    }

    // Opacify when hitting inside_length limit
    if (inside_length >= transparency_depth) {
        result.a = 1.0;
    } else if (inside_length >= transparency_depth * 0.8) {
        result.a += (1.0 - result.a) * ((inside_length - transparency_depth * 0.8) / (transparency_depth * 0.2));
    }

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
        return false;
    }

    double ymin, ymax;
    model->getAltitudeRange(&ymin, &ymax);
    double light_traversal = (ymax - ymin) * 0.8 * light->color.getPower();
    findSegments(model, start, direction, 30, light_traversal, end.sub(start).getNorm(), &inside_depth, &total_depth,
                 segments);

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
