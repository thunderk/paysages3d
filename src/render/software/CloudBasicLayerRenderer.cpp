#include "CloudBasicLayerRenderer.h"

#include "CloudLayerDefinition.h"
#include "SoftwareRenderer.h"
#include "NoiseGenerator.h"
#include "Curve.h"
#include "AtmosphereRenderer.h"

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

static inline double _standardCoverageFunc(CloudLayerDefinition* layer, Vector3 position)
{
    if (position.y < layer->lower_altitude || position.y > (layer->lower_altitude + layer->thickness))
    {
        return 0.0;
    }
    else
    {
        return layer->base_coverage * layer->_coverage_by_altitude->getValue((position.y - layer->lower_altitude) / layer->thickness);
    }
}

static inline double _getDistanceToBorder(CloudLayerDefinition* layer, Vector3 position)
{
    double val;
    double minval, maxval;

    layer->_shape_noise->getRange(&minval, &maxval);

    val = 0.5 * layer->_shape_noise->get3DTotal(position.x / layer->shape_scaling, position.y / layer->shape_scaling, position.z / layer->shape_scaling) / maxval;

    return (val - 0.5 + _standardCoverageFunc(layer, position)) * layer->shape_scaling;
}

static inline Vector3 _getNormal(CloudLayerDefinition* layer, Vector3 position, double detail)
{
    Vector3 result = {0.0, 0.0, 0.0};
    Vector3 dposition;
    double val, dval;

    val = _getDistanceToBorder(layer, position);

    dposition.x = position.x + detail;
    dposition.y = position.y;
    dposition.z = position.z;
    dval = val - _getDistanceToBorder(layer, dposition);
    result.x += dval;

    dposition.x = position.x - detail;
    dval = val - _getDistanceToBorder(layer, dposition);
    result.x -= dval;

    dposition.x = position.x;
    dposition.y = position.y + detail;
    dval = val - _getDistanceToBorder(layer, dposition);
    result.y += dval;

    dposition.y = position.y - detail;
    dval = val - _getDistanceToBorder(layer, dposition);
    result.y -= dval;

    dposition.y = position.y;
    dposition.z = position.z + detail;
    dval = val - _getDistanceToBorder(layer, dposition);
    result.z += dval;

    dposition.z = position.z - detail;
    dval = val - _getDistanceToBorder(layer, dposition);
    result.z -= dval;

    return v3Normalize(result);
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
static int _findSegments(CloudLayerDefinition* definition, SoftwareRenderer* renderer, Vector3 start, Vector3 direction, double, int max_segments, double max_inside_length, double max_total_length, double* inside_length, double* total_length, CloudSegment* out_segments)
{
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

    render_precision = 15.2 - 1.5 * (double)renderer->render_quality;
    render_precision = render_precision * definition->shape_scaling / 50.0;
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
    noise_distance = _getDistanceToBorder(definition, start) * render_precision;
    inside = (noise_distance > 0.0) ? 1 : 0;
    step = v3Scale(direction, render_precision);

    do
    {
        walker = v3Add(walker, step);
        step_length = v3Norm(step);
        last_noise_distance = noise_distance;
        noise_distance = _getDistanceToBorder(definition, walker) * render_precision;
        current_total_length += step_length;

        if (noise_distance > 0.0)
        {
            if (inside)
            {
                // inside the cloud
                segment_length += step_length;
                current_inside_length += step_length;
                step = v3Scale(direction, (noise_distance < render_precision) ? render_precision : noise_distance);
            }
            else
            {
                // entering the cloud
                inside = 1;
                segment_length = step_length * noise_distance / (noise_distance - last_noise_distance);
                segment_start = v3Add(walker, v3Scale(direction, -segment_length));
                current_inside_length += segment_length;
                step = v3Scale(direction, render_precision);
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
                out_segments->end = v3Add(walker, v3Scale(direction, remaining_length - step_length));
                out_segments->length = segment_length;
                out_segments++;
                if (++segment_count >= max_segments)
                {
                    break;
                }

                inside = 0;
                step = v3Scale(direction, render_precision);
            }
            else
            {
                // searching for a cloud
                step = v3Scale(direction, (noise_distance > -render_precision) ? render_precision : -noise_distance);
            }
        }
    } while (inside || (walker.y >= definition->lower_altitude - 0.001 && walker.y <= (definition->lower_altitude + definition->thickness) + 0.001 && current_total_length < max_total_length && current_inside_length < max_inside_length));

    *total_length = current_total_length;
    *inside_length = current_inside_length;
    return segment_count;
}

static Color _applyLayerLighting(CloudLayerDefinition* definition, SoftwareRenderer* renderer, Vector3 position, double)
{
    Vector3 normal;
    Color col1, col2;

    normal = _getNormal(definition, position, 3.0);
    if (renderer->render_quality > 5)
    {
        normal = v3Add(normal, _getNormal(definition, position, 2.0));
        normal = v3Add(normal, _getNormal(definition, position, 1.0));
    }
    if (renderer->render_quality > 5)
    {
        normal = v3Add(normal, _getNormal(definition, position, 0.5));
    }
    normal = v3Scale(v3Normalize(normal), definition->hardness);

    // TODO Compute light filter only once
    col1 = renderer->applyLightingToSurface(renderer, position, normal, definition->material);
    col2 = renderer->applyLightingToSurface(renderer, position, v3Scale(normal, -1.0), definition->material);

    col1.r = (col1.r + col2.r) / 2.0;
    col1.g = (col1.g + col2.g) / 2.0;
    col1.b = (col1.b + col2.b) / 2.0;
    col1.a = (col1.a + col2.a) / 2.0;

    return col1;
}

double CloudBasicLayerRenderer::getDensity(CloudLayerDefinition* layer, const Vector3 &location)
{
    return 0.0;
}

Color CloudBasicLayerRenderer::getColor(CloudLayerDefinition* layer, const Vector3 &eye, const Vector3 &location)
{
    int i, segment_count;
    double max_length, detail, total_length, inside_length;
    Vector3 start, end, direction;
    Color result, col;
    CloudSegment segments[20];

    start = eye;
    end = location;
    if (!optimizeSearchLimits(layer, &start, &end))
    {
        return COLOR_TRANSPARENT;
    }

    direction = end.sub(start);
    max_length = direction.getNorm();
    direction = direction.normalize();
    result = COLOR_TRANSPARENT;

    detail = parent->getPrecision(parent, start) / layer->shape_scaling;

    segment_count = _findSegments(layer, parent, start, direction, detail, 20, layer->transparencydepth, max_length, &inside_length, &total_length, segments);
    for (i = segment_count - 1; i >= 0; i--)
    {
        col = _applyLayerLighting(layer, parent, segments[i].start, detail);
        col.a = (segments[i].length >= layer->transparencydepth) ? 1.0 : (segments[i].length / layer->transparencydepth);
        colorMask(&result, &col);
    }
    if (inside_length >= layer->transparencydepth)
    {
        result.a = 1.0;
    }

    double a = result.a;
    result = parent->getAtmosphereRenderer()->applyAerialPerspective(start, result).final;
    result.a = a;

    return result;
}

bool CloudBasicLayerRenderer::alterLight(CloudLayerDefinition* layer, LightDefinition* light, const Vector3 &, const Vector3 &location)
{
    Vector3 start, end;
    double inside_depth, total_depth, factor;
    CloudSegment segments[20];

    start = location;
    end = location.add(light->direction.scale(10000.0));
    if (not optimizeSearchLimits(layer, &start, &end))
    {
        return false;
    }

    _findSegments(layer, parent, start, light->direction, 0.1, 20, layer->lighttraversal, end.sub(start).getNorm(), &inside_depth, &total_depth, segments);

    if (layer->lighttraversal < 0.0001)
    {
        factor = 0.0;
    }
    else
    {
        factor = inside_depth / layer->lighttraversal;
        if (factor > 1.0)
        {
            factor = 1.0;
        }
    }

    factor = 1.0 - (1.0 - layer->minimumlight) * factor;

    light->color.r *= factor;
    light->color.g *= factor;
    light->color.b *= factor;

    return true;
}
