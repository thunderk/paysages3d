#include "CloudBasicLayerRenderer.h"

#include "CloudLayerDefinition.h"
#include "SoftwareRenderer.h"
#include "NoiseGenerator.h"
#include "Curve.h"
#include "AtmosphereRenderer.h"
#include "AtmosphereResult.h"
#include "LightComponent.h"
#include "clouds/BaseCloudsModel.h"
#include "SurfaceMaterial.h"

typedef struct
{
    Vector3 start;
    Vector3 end;
    double length;
} CloudSegment;

CloudBasicLayerRenderer::CloudBasicLayerRenderer(SoftwareRenderer* parent):
    BaseCloudLayerRenderer(parent)
{
}

static inline double _getDistanceToBorder(BaseCloudsModel* model, Vector3 position)
{
    return model->getDensity(position);
}

/**
 * Go through the cloud layer to find segments (parts of the lookup that are inside the cloud).
 *
 * @param definition The cloud layer
 * @param renderer The renderer environment
 * @param start Start position of the lookup (already optimized)
 * @param direction Normalized direction of the lookup
 * @param detail Level of noise detail required
 * @param max_segments Maximum number of segments to collect
 * @param max_inside_length Maximum length to spend inside the cloud
 * @param max_total_length Maximum lookup length
 * @param inside_length Resulting length inside cloud (sum of all segments length)
 * @param total_length Resulting lookup length
 * @param out_segments Allocated space to fill found segments
 * @return Number of segments found
 */
static int _findSegments(BaseCloudsModel* model, SoftwareRenderer* renderer, Vector3 start, Vector3 direction, double, int max_segments, double max_inside_length, double max_total_length, double* inside_length, double* total_length, CloudSegment* out_segments)
{
    CloudLayerDefinition* layer = model->getLayer();
    double ymin, ymax;
    int inside, segment_count;
    double current_total_length, current_inside_length;
    double step_length, segment_length, remaining_length;
    double noise_distance, last_noise_distance;
    Vector3 walker, step, segment_start;
    double render_precision;

    if (max_segments <= 0)
    {
        return 0;
    }

    model->getAltitudeRange(&ymin, &ymax);

    render_precision = 15.2 - 1.5 * (double)renderer->render_quality;
    render_precision = render_precision * layer->scaling / 50.0;
    if (render_precision > max_total_length / 10.0)
    {
        render_precision = max_total_length / 10.0;
    }
    else if (render_precision < max_total_length / 2000.0)
    {
        render_precision = max_total_length / 2000.0;
    }

    segment_count = 0;
    current_total_length = 0.0;
    current_inside_length = 0.0;
    segment_length = 0.0;
    walker = start;
    noise_distance = _getDistanceToBorder(model, start) * render_precision;
    inside = 0;
    step = direction.scale(render_precision);

    do
    {
        walker = walker.add(step);
        step_length = step.getNorm();
        last_noise_distance = noise_distance;
        noise_distance = _getDistanceToBorder(model, walker) * render_precision;
        current_total_length += step_length;

        if (noise_distance > 0.0)
        {
            if (inside)
            {
                // inside the cloud
                segment_length += step_length;
                current_inside_length += step_length;
                step = direction.scale((noise_distance < render_precision) ? render_precision : noise_distance);
            }
            else
            {
                // entering the cloud
                inside = 1;
                segment_length = step_length * noise_distance / (noise_distance - last_noise_distance);
                segment_start = walker.add(direction.scale(-segment_length));
                current_inside_length += segment_length;
                step = direction.scale(render_precision);
            }
        }
        else
        {
            if (inside)
            {
                // exiting the cloud
                remaining_length = step_length * last_noise_distance / (last_noise_distance - noise_distance);
                segment_length += remaining_length;
                current_inside_length += remaining_length;

                out_segments->start = segment_start;
                out_segments->end = walker.add(direction.scale(remaining_length - step_length));
                out_segments->length = segment_length;
                out_segments++;
                if (++segment_count >= max_segments)
                {
                    break;
                }

                inside = 0;
                step = direction.scale(render_precision);
            }
            else
            {
                // searching for a cloud
                step = direction.scale((noise_distance > -render_precision) ? render_precision : -noise_distance);
            }
        }
    } while (inside || (walker.y >= ymin - 0.001 && walker.y <= ymax + 0.001 && current_total_length < max_total_length && current_inside_length < max_inside_length));

    *total_length = current_total_length;
    *inside_length = current_inside_length;
    return segment_count;
}

Color CloudBasicLayerRenderer::getColor(BaseCloudsModel *model, const Vector3 &eye, const Vector3 &location)
{
    CloudLayerDefinition* layer = model->getLayer();
    int i, segment_count;
    double max_length, detail, total_length, inside_length;
    Vector3 start, end, direction;
    Color result, col;
    CloudSegment segments[20];

    start = eye;
    end = location;
    if (!optimizeSearchLimits(model, &start, &end))
    {
        return COLOR_TRANSPARENT;
    }

    direction = end.sub(start);
    max_length = direction.getNorm();
    direction = direction.normalize();
    result = COLOR_TRANSPARENT;

    detail = parent->getPrecision(start) / layer->scaling;
    double transparency_depth = layer->scaling * 0.5;

    segment_count = _findSegments(model, parent, start, direction, detail, 20, transparency_depth, max_length, &inside_length, &total_length, segments);
    for (i = segment_count - 1; i >= 0; i--)
    {
        SurfaceMaterial material;
        material.base = colorToHSL(Color(0.7, 0.7, 0.7));
        material.hardness = 0.25;
        material.reflection = 0.0;
        material.shininess = 0.0;
        materialValidate(&material);

        col = parent->applyLightingToSurface(segments[i].start, parent->getAtmosphereRenderer()->getSunDirection(), material);

        col.a = (segments[i].length >= transparency_depth) ? 1.0 : (segments[i].length / transparency_depth);
        result.mask(col);
    }
    if (inside_length >= transparency_depth)
    {
        result.a = 1.0;
    }

    double a = result.a;
    result = parent->getAtmosphereRenderer()->applyAerialPerspective(start, result).final;
    result.a = a;

    return result;
}

bool CloudBasicLayerRenderer::alterLight(BaseCloudsModel *model, LightComponent* light, const Vector3 &, const Vector3 &location)
{
    Vector3 start, end;
    double inside_depth, total_depth, factor;
    CloudSegment segments[20];

    start = location;
    end = location.add(light->direction.scale(10000.0));
    if (not optimizeSearchLimits(model, &start, &end))
    {
        return false;
    }

    double light_traversal = model->getLayer()->scaling * 8.0;
    _findSegments(model, parent, start, light->direction, 0.1, 20, light_traversal, end.sub(start).getNorm(), &inside_depth, &total_depth, segments);

    if (light_traversal < 0.0001)
    {
        factor = 0.0;
    }
    else
    {
        factor = inside_depth / light_traversal;
        if (factor > 1.0)
        {
            factor = 1.0;
        }
    }

    double miminum_light = 0.4;
    factor = 1.0 - (1.0 - miminum_light) * factor;

    light->color.r *= factor;
    light->color.g *= factor;
    light->color.b *= factor;

    return true;
}
